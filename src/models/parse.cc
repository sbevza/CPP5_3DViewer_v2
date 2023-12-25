#include "parse.h"

namespace s21 {

void Parser::parseObj(Attrib &attrib, std::string &filename) {
  err_ = false;
  attrib_ = &attrib;
  filename_ = filename;
  std::vector<char> buf{};
  try {
    buf = getFileData();
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << '\n';
    err_ = true;
  }

  if (!err_) err_ = buf.empty();
  attribInit();

  std::vector<LineInfo> lineInfos;
  size_t lines = getLineInfos(buf, lineInfos);
  std::vector<Command> commands;
  if (!err_) commands.resize(lines);

  size_t numFaces = 0;
  size_t numV = 0;
  size_t numF = 0;
  for (size_t i = 0; i < lines && !err_; i++) {
    int res = 0;
    std::string line = std::string(
        buf.begin() + static_cast<std::ptrdiff_t>(lineInfos[i].pos),
        buf.begin() + static_cast<std::ptrdiff_t>(lineInfos[i].pos) +
            static_cast<std::ptrdiff_t>(lineInfos[i].len));
    parseLine(commands[i], line, res);
    if (!err_ && res) {
      if (commands[i].type == CommandType::V) {
        numV++;
      } else if (commands[i].type == CommandType::F) {
        numF += commands[i].numF;
        numFaces += commands[i].numFaceNumVerts;
      }
    }
  }
  if (!err_) {
    setAttrib(numV, numF, numFaces);
    commandToAttrib(commands);
  }
}

void Parser::attribInit() {
  attrib_->vertices.clear();
  attrib_->verticesShade.clear();
  attrib_->vertexTexture.clear();
  attrib_->vertexNormal.clear();
  attrib_->vertexNormalShade.clear();
  attrib_->faces.clear();
  attrib_->facesShade.clear();
  attrib_->vtIdx.clear();
  attrib_->vnIdx.clear();
  attrib_->numVertices = 0;
  attrib_->numFaceNumVerts = 0;
  attrib_->minX = FLT_MAX;
  attrib_->maxX = -FLT_MAX;
  attrib_->minY = FLT_MAX;
  attrib_->maxY = -FLT_MAX;
  attrib_->minZ = FLT_MAX;
  attrib_->maxZ = -FLT_MAX;
  uniqueFace_.clear();
  uniqueFaceShade_.clear();
}

std::vector<char> Parser::getFileData() {
  if (filename_.empty()) {
    throw std::runtime_error("Error: Invalid filename");
  }

  std::ifstream file(filename_, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filename_);
  }

  std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

  if (buffer.empty()) {
    throw std::runtime_error("The file is empty");
  }

  return buffer;

}

int Parser::isLineEnding(std::vector<char> &ch, size_t i, size_t end_i) {
  return (ch[i] == '\0') || (ch[i] == '\n') ||
      ((ch[i] == '\r') && (((i + 1) < end_i) && (ch[i + 1] != '\n')));
}

size_t Parser::getLineInfos(std::vector<char> &buf,
                            std::vector<LineInfo> &lineInfos) {
  size_t endIdx = buf.size();
  size_t lastLineEnding = 0;
  size_t lines = 0;

  for (size_t i = 0; i < endIdx; i++) {
    if (isLineEnding(buf, i, endIdx)) {
      lines++;
      lastLineEnding = i;
    }
  }

  if (endIdx - lastLineEnding > 0) {
    lines++;
  }

  err_ = (lines == 0);
  if (!err_) {
    lineInfos.resize(lines);
    size_t line_no = 0;
    size_t prev_pos = 0;
    for (size_t i = 0; i < endIdx; i++) {
      if (isLineEnding(buf, i, endIdx)) {
        lineInfos[line_no].pos = prev_pos;
        lineInfos[line_no].len = i - prev_pos;
        prev_pos = i + 1;
        line_no++;
      }
    }
    if (endIdx - lastLineEnding > 0) {
      lineInfos[line_no].pos = prev_pos;
      lineInfos[line_no].len = endIdx - 1 - lastLineEnding;
    }
  }
  return lines;
}

void Parser::skipSpace(const std::string &str, size_t &pos) {
  while (pos < str.length() && std::isspace(str[pos])) {
    pos++;
  }
}

float Parser::parseDouble(const std::string &str, size_t &pos) {
  skipSpace(str, pos);
  size_t nextPos;
  float value = std::stof(str.substr(pos), &nextPos);
  pos += nextPos;
  return value;
}

VertexIndex Parser::parseRawTriple(const std::string &str, size_t &pos) {
  VertexIndex vi{};
  vi.vIdx = 0;
  vi.vtIdx = -1;
  vi.vnIdx = -1;

  size_t nextPos = str.find_first_of(" \t\r", pos);
  std::string subStr = str.substr(pos, nextPos - pos);

  size_t firstSlash = subStr.find_first_of('/');
  if (firstSlash != std::string::npos) {
    vi.vIdx = std::stoi(subStr.substr(0, firstSlash));
    size_t secondSlash = subStr.find_first_of('/', firstSlash + 1);
    if (secondSlash != std::string::npos) {
      if (secondSlash != firstSlash + 1) {
        vi.vtIdx = std::stoi(subStr.substr(firstSlash + 1, secondSlash - firstSlash - 1));
      }
      if (secondSlash + 1 < subStr.size()) {
        vi.vnIdx = std::stoi(subStr.substr(secondSlash + 1));
      }
    } else {
      if (firstSlash + 1 < subStr.size()) {
        vi.vtIdx = std::stoi(subStr.substr(firstSlash + 1));
      }
    }
  } else {
    vi.vIdx = std::stoi(subStr);
  }

  pos = (nextPos == std::string::npos) ? str.length() : nextPos;
  return vi;
}

void Parser::parseLine(Command &command, const std::string &line, int &res) {
  res = 0;
  if (line.size() >= kMaxLineBuffer) {
    err_ = true;
  } else {
    command.type = CommandType::EMPTY;
    size_t pos = 0;
    skipSpace(line, pos);

    if (line.compare(pos, 2, "v ") == 0 || line.compare(pos, 2, "v\t") == 0) {
      parseVertexCommand(command, line, pos, res);
    } else if (line.compare(pos, 3, "vt ") == 0 || line.compare(pos, 3, "vt\t") == 0) {
      parseTextureCommand(command, line, pos, res);
    } else if (line.compare(pos, 3, "vn ") == 0 || line.compare(pos, 3, "vn\t") == 0) {
      parseNormalCommand(command, line, pos, res);
    } else if (line.compare(pos, 2, "f ") == 0 || line.compare(pos, 2, "f\t") == 0) {
      parseFaceCommand(command, line, pos, res);
    }
  }
}

void Parser::parseVertexCommand(Command &command, const std::string &line, size_t &pos, int &res) {
  pos += 2;
  command.vx = parseDouble(line, pos);
  command.vy = parseDouble(line, pos);
  command.vz = parseDouble(line, pos);
  command.type = CommandType::V;
  res = 1;
}

void Parser::parseTextureCommand(Command &command, const std::string &line, size_t &pos, int &res) {
  pos += 3;
  command.vtU = parseDouble(line, pos);
  command.vtV = parseDouble(line, pos);
  command.type = CommandType::VT;
  res = 1;
}

void Parser::parseNormalCommand(Command &command, const std::string &line, size_t &pos, int &res) {
  pos += 3;
  command.vnX = parseDouble(line, pos);
  command.vnY = parseDouble(line, pos);
  command.vnZ = parseDouble(line, pos);
  command.type = CommandType::VN;
  res = 1;
}

void Parser::parseFaceCommand(Command &command, const std::string &line, size_t &pos, int &res) {
  pos += 2;
  skipSpace(line, pos);
  size_t num_f = 0;
  std::vector<VertexIndex> f;

  while (!(line[pos] == '\r' || line[pos] == '\n' || line[pos] == '\0')) {
    f.push_back(parseRawTriple(line, pos));
    num_f++;
    skipSpace(line, pos);
  }

  command.type = CommandType::F;

  size_t k;
  VertexIndex i0 = f[0];
  VertexIndex i1{};
  VertexIndex i2 = f[1];
  for (k = 2; k < num_f; k++) {
    i1 = i2;
    i2 = f[k];
    command.fShade.push_back({i0.vIdx, i0.vtIdx, i0.vnIdx});
    command.fShade.push_back({i1.vIdx, i1.vtIdx, i1.vnIdx});
    command.fShade.push_back({i2.vIdx, i2.vtIdx, i2.vnIdx});
  }

  for (size_t k = 0; k < num_f; k++) {
    command.f.push_back(f[k].vIdx);
    command.vtIdx.push_back(f[k].vtIdx);
    command.vnIdx.push_back(f[k].vnIdx);
  }
  command.numF = num_f;
  command.numFaceNumVerts = 1;
  res = 1;
}

size_t Parser::fixIndex(int idx, size_t num_v) {
  size_t ret;
  if (idx > 0)
    ret = idx - 1;
  else if (idx == 0)
    ret = 0;
  else
    ret = num_v + idx;
  return ret;
}

void Parser::setAttrib(size_t num_v, size_t num_f, size_t num_faces) {
  if (num_v) {
    attrib_->numVertices = static_cast<unsigned int>(num_v);
    attrib_->numFaceNumVerts = static_cast<unsigned int>(num_faces);
    attrib_->vertices = std::vector<float>(num_v * 3);
  } else {
    err_ = true;
  }
}

bool Parser::hasError() const { return err_; }

void Parser::commandToAttrib(const std::vector<Command> &commands) {
  size_t v_count = 0;
  size_t f_count = 0;
  float minX = FLT_MAX, maxX = -FLT_MAX;
  float minY = FLT_MAX, maxY = -FLT_MAX;
  float minZ = FLT_MAX, maxZ = -FLT_MAX;

  calculateBounds(commands, minX, maxX, minY, maxY, minZ, maxZ);

  float centerX = (minX + maxX) / 2.0f;
  float centerY = (minY + maxY) / 2.0f;
  float centerZ = (minZ + maxZ) / 2.0f;

  for (const auto &command : commands) {
    if (command.type == CommandType::V) {
      processVertex(command, centerX, centerY, centerZ, v_count);
    } else if (command.type == CommandType::F) {
      processFace(command, f_count, v_count);
    } else if (command.type == CommandType::VT) {
      processTexture(command);
    } else if (command.type == CommandType::VN) {
      processNormal(command);
    }
  }

  attrib_->numVertices = v_count;
  attrib_->minX = minX - centerX;
  attrib_->maxX = maxX - centerX;
  attrib_->minY = minY - centerY;
  attrib_->maxY = maxY - centerY;
  attrib_->minZ = minZ - centerZ;
  attrib_->maxZ = maxZ - centerZ;
  for (const auto &face : uniqueFace_) {
    attrib_->faces.push_back(face.first);
    attrib_->faces.push_back(face.second);
  }
  for (const auto &face : uniqueFaceShade_) {
    int vertexIndex, textureIndex, normalIndex;
    std::tie(vertexIndex, textureIndex, normalIndex) = face;

    attrib_->facesShade.push_back(vertexIndex);

    attrib_->verticesShade.push_back(attrib_->vertices[3 * (vertexIndex)]);
    attrib_->verticesShade.push_back(attrib_->vertices[3 * (vertexIndex) + 1]);
    attrib_->verticesShade.push_back(attrib_->vertices[3 * (vertexIndex) + 2]);

    attrib_->vertexTextureShade.push_back(attrib_->vertexTexture[2 * textureIndex]);
    attrib_->vertexTextureShade.push_back(attrib_->vertexTexture[2 * textureIndex + 1]);

    attrib_->vertexNormalShade.push_back(attrib_->vertexNormal[3 * (normalIndex)]);
    attrib_->vertexNormalShade.push_back(attrib_->vertexNormal[3 * (normalIndex) + 1]);
    attrib_->vertexNormalShade.push_back(attrib_->vertexNormal[3 * (normalIndex) + 2]);
  }
}

void Parser::processNormal(const Command &command) {
  attrib_->vertexNormal.push_back(command.vnX);
  attrib_->vertexNormal.push_back(command.vnY);
  attrib_->vertexNormal.push_back(command.vnZ);
}

void Parser::processTexture(const Command &command) {
  attrib_->vertexTexture.push_back(command.vtU);
  attrib_->vertexTexture.push_back(command.vtV);
}

void Parser::calculateBounds(const std::vector<Command> &commands, float &minX,
                             float &maxX, float &minY, float &maxY, float &minZ,
                             float &maxZ) {
  for (const auto &command : commands) {
    if (command.type == CommandType::V) {
      minX = std::min(minX, command.vx);
      maxX = std::max(maxX, command.vx);
      minY = std::min(minY, command.vy);
      maxY = std::max(maxY, command.vy);
      minZ = std::min(minZ, command.vz);
      maxZ = std::max(maxZ, command.vz);
    }
  }
}

void Parser::processVertex(const Command &command, float centerX, float centerY,
                           float centerZ, size_t &v_count) {
  attrib_->vertices[3 * v_count + 0] = command.vx - centerX;
  attrib_->vertices[3 * v_count + 1] = command.vy - centerY;
  attrib_->vertices[3 * v_count + 2] = command.vz - centerZ;
  v_count++;
}

void Parser::processFace(const Command &command, size_t &f_count,
                         size_t v_count) {

  if (attrib_->vnIdx.size() != v_count) {
    attrib_->vnIdx.resize(v_count);
    attrib_->vtIdx.resize(v_count);
  }

  if (command.numF > 0) {
    size_t k = 0;
    size_t previous_v_idx = fixIndex(command.f[k++], v_count);
    for (; k < command.numF; ++k) {
      attrib_->vnIdx[previous_v_idx] = fixIndex(command.vnIdx[k - 1], v_count);
      attrib_->vtIdx[previous_v_idx] = fixIndex(command.vtIdx[k - 1], v_count);
      size_t v_idx = fixIndex(command.f[k], v_count);
      auto result
          = uniqueFace_.insert(std::make_pair(std::min(previous_v_idx, v_idx),
                                              std::max(previous_v_idx, v_idx)));

      if (result.second) {
        f_count += 2;
      }
      previous_v_idx = v_idx;
    }

    attrib_->vnIdx[previous_v_idx] = fixIndex(command.vnIdx[k - 1], v_count);
    attrib_->vtIdx[previous_v_idx] = fixIndex(command.vtIdx[k - 1], v_count);
    // Замыкание грани
    size_t v_idx = fixIndex(command.f[0], v_count);
    auto result = uniqueFace_.insert(std::make_pair(std::min(previous_v_idx, v_idx), std::max(previous_v_idx, v_idx)));
    if (result.second) {
      f_count += 2;
    }

    if (!command.fShade.empty()) {
      for (auto f : command.fShade) {
//        attrib_->facesShade.push_back(fixIndex(std::get<0>(f), v_count));
        uniqueFaceShade_.push_back({fixIndex(std::get<0>(f), v_count),
                                    fixIndex(std::get<1>(f), v_count),
                                    fixIndex(std::get<2>(f), v_count)});
      }
    }
  }
}

}  // namespace s21
