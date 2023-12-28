#include "parse.h"

namespace s21 {

void Parser::parseObj(Attrib &attrib, std::string &filename) {
  attrib_ = &attrib;
  err_ = false;
  attribInit();
  std::vector<char> buf{};
  try {
    buf = getFileData(filename);
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << '\n';
    err_ = true;
  }
  std::vector<LineInfo> lineInfos = getLineInfos(buf);

  std::vector<Command> commands;
  size_t numV = 0;
  for (const auto &lineInfo : lineInfos) {
    processLine(commands, buf, lineInfo, numV);
  }

  if (!commands.empty()) {
    setAttrib(numV);
    commandToAttrib(commands);
  }
}

void Parser::processLine(std::vector<Command> &commands, const std::vector<char> &buf,
                         const LineInfo &lineInfo, size_t &numV) {
  std::string line = std::string(buf.begin() + static_cast<std::ptrdiff_t>(lineInfo.pos),
                                 buf.begin() + static_cast<std::ptrdiff_t>(lineInfo.pos) +
                                     static_cast<std::ptrdiff_t>(lineInfo.len));
  Command command;
  if (parseLine(command, line)) {
    if (command.type == CommandType::V) {
      numV++;
    }
    commands.push_back(std::move(command));
  }
}

void Parser::attribInit() {
  attrib_->vertices.clear();
  attrib_->verticesShade.clear();
  attrib_->vertexTexture.clear();
  attrib_->vertexNormal.clear();
  attrib_->vertexNormalShade.clear();
  attrib_->faces.clear();
  attrib_->minX = std::numeric_limits<float>::max();
  attrib_->maxX = -std::numeric_limits<float>::max();
  attrib_->minY = std::numeric_limits<float>::max();
  attrib_->maxY = -std::numeric_limits<float>::max();
  attrib_->minZ = std::numeric_limits<float>::max();
  attrib_->maxZ = -std::numeric_limits<float>::max();
  uniqueFace_.clear();
  uniqueFaceShade_.clear();
}

std::vector<char> Parser::getFileData(const std::string &filename) {
  std::filesystem::path filePath(filename);
  if (!std::filesystem::exists(filePath)) {
    throw std::runtime_error("Error: Invalid filename");
  }

  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

  if (buffer.empty()) {
    throw std::runtime_error("The file is empty");
  }

  return buffer;
}

int Parser::isLineEnding(const std::vector<char> &ch, size_t i, size_t end_i) {
  return (ch[i] == '\0') || (ch[i] == '\n') ||
      ((ch[i] == '\r') && (((i + 1) < end_i) && (ch[i + 1] != '\n')));
}

std::vector<LineInfo> Parser::getLineInfos(const std::vector<char> &buf) {
  std::vector<LineInfo> lineInfos;
  size_t endIdx = buf.size();
  size_t lineStart = 0;

  for (size_t i = 0; i < endIdx; ++i) {
    if (isLineEnding(buf, i, endIdx)) {
      lineInfos.emplace_back(LineInfo{lineStart, i - lineStart});
      lineStart = i + 1;
    }
  }

  if (lineStart < endIdx) {
    lineInfos.emplace_back(LineInfo{lineStart, endIdx - lineStart});
  }

  return lineInfos;
}

VertexIndex Parser::parseRawTriple(const std::string &str, size_t &pos) {
  VertexIndex vi{};
  vi.vIdx = 0;
  vi.vtIdx = 0;
  vi.vnIdx = 0;

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

bool Parser::parseLine(Command &command, const std::string &line) {
  std::istringstream iss(line);
  std::string token;
  bool result = false;

  iss >> token;
  if (!token.empty()) {
    if (token == "v") {
      command.type = CommandType::V;
      iss >> command.vx >> command.vy >> command.vz;
      result = true;
    } else if (token == "vt") {
      command.type = CommandType::VT;
      iss >> command.vtU >> command.vtV;
      result = true;
    } else if (token == "vn") {
      command.type = CommandType::VN;
      iss >> command.vnX >> command.vnY >> command.vnZ;
      result = true;
    } else if (token == "f") {
      parseFaceCommand(command, iss);
      result = true;
    }
  }

  return result;
}

void Parser::parseFaceCommand(Command &command, std::istringstream &iss) {
  command.type = CommandType::F;
  std::string token;
  std::vector<VertexIndex> f;

  while (iss >> token) {
    size_t a = 0;
    f.push_back(parseRawTriple(token, a));
  }

  if (f.size() >= 3) {
    VertexIndex i0 = f[0];
    VertexIndex i1 = {0, 0, 0};
    VertexIndex i2 = {0, 0, 0};

    for (size_t k = 2; k < f.size(); k++) {
      i1 = f[k - 1];
      i2 = f[k];
      command.fShade.emplace_back(i0.vIdx, i0.vtIdx, i0.vnIdx);
      command.fShade.emplace_back(i1.vIdx, i1.vtIdx, i1.vnIdx);
      command.fShade.emplace_back(i2.vIdx, i2.vtIdx, i2.vnIdx);
    }

    for (const auto &vi : f) {
      command.f.push_back(vi.vIdx);
    }
  }
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

void Parser::setAttrib(size_t num_v) {
  if (num_v) {
    attrib_->vertices = std::vector<float>(num_v * 3);
  } else {
    err_ = true;
  }
}

bool Parser::hasError() const { return err_; }

void Parser::commandToAttrib(const std::vector<Command> &commands) {
  size_t v_count = 0;
  Bounds bounds;

  calculateBounds(commands, bounds);
  float centerX = (bounds.minX + bounds.maxX) / 2.0f;
  float centerY = (bounds.minY + bounds.maxY) / 2.0f;
  float centerZ = (bounds.minZ + bounds.maxZ) / 2.0f;

  for (const auto &command : commands) {
    if (command.type == CommandType::V) {
      processVertex(command, centerX, centerY, centerZ, v_count);
    } else if (command.type == CommandType::F) {
      processFace(command, v_count);
    } else if (command.type == CommandType::VT) {
      processTexture(command);
    } else if (command.type == CommandType::VN) {
      processNormal(command);
    }
  }

  updateAttribPositions(centerX, centerY, centerZ, bounds);
  calculateShadeModel();
  recalculateNormals();
}

void Parser::updateAttribPositions(float centerX, float centerY, float centerZ, Bounds &bounds) {
  attrib_->minX = bounds.minX - centerX;
  attrib_->maxX = bounds.maxX - centerX;
  attrib_->minY = bounds.minY - centerY;
  attrib_->maxY = bounds.maxY - centerY;
  attrib_->minZ = bounds.minZ - centerZ;
  attrib_->maxZ = bounds.maxZ - centerZ;

  for (const auto &face : uniqueFace_) {
    attrib_->faces.push_back(face.first);
    attrib_->faces.push_back(face.second);
  }
}

void Parser::calculateShadeModel() {
  for (const auto &[vertexIndex, textureIndex, normalIndex] : uniqueFaceShade_) {
    attrib_->verticesShade.push_back(attrib_->vertices[3 * vertexIndex]);
    attrib_->verticesShade.push_back(attrib_->vertices[3 * vertexIndex + 1]);
    attrib_->verticesShade.push_back(attrib_->vertices[3 * vertexIndex + 2]);

    if (!attrib_->vertexTexture.empty()) {
      attrib_->vertexTextureShade.push_back(attrib_->vertexTexture[2 * textureIndex]);
      attrib_->vertexTextureShade.push_back(attrib_->vertexTexture[2 * textureIndex + 1]);
    }

    if (!attrib_->vertexNormal.empty()) {
      attrib_->vertexNormalShade.push_back(attrib_->vertexNormal[3 * normalIndex]);
      attrib_->vertexNormalShade.push_back(attrib_->vertexNormal[3 * normalIndex + 1]);
      attrib_->vertexNormalShade.push_back(attrib_->vertexNormal[3 * normalIndex + 2]);
    }
  }
}

void Parser::recalculateNormals() {
  if (attrib_->vertexNormal.empty()) {
    for (size_t i = 0; i < attrib_->verticesShade.size(); i += 9) {
      Vertex v1 = {attrib_->verticesShade[i], attrib_->verticesShade[i + 1], attrib_->verticesShade[i + 2]};
      Vertex v2 = {attrib_->verticesShade[i + 3], attrib_->verticesShade[i + 4], attrib_->verticesShade[i + 5]};
      Vertex v3 = {attrib_->verticesShade[i + 6], attrib_->verticesShade[i + 7], attrib_->verticesShade[i + 8]};

      Vertex normal = calculateNormal(v1, v2, v3);

      for (int j = 0; j < 3; ++j) {
        attrib_->vertexNormalShade.push_back(normal.x);
        attrib_->vertexNormalShade.push_back(normal.y);
        attrib_->vertexNormalShade.push_back(normal.z);
      }
    }
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

void Parser::calculateBounds(const std::vector<Command> &commands, Bounds &bounds) {
  for (const auto &command : commands) {
    if (command.type == CommandType::V) {
      bounds.minX = std::min(bounds.minX, command.vx);
      bounds.maxX = std::max(bounds.maxX, command.vx);
      bounds.minY = std::min(bounds.minY, command.vy);
      bounds.maxY = std::max(bounds.maxY, command.vy);
      bounds.minZ = std::min(bounds.minZ, command.vz);
      bounds.maxZ = std::max(bounds.maxZ, command.vz);
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

void Parser::processFace(const Command &command, size_t v_count) {
  if (!command.f.empty()) {
    size_t k = 0;
    size_t previous_v_idx = fixIndex(command.f[k++], v_count);
    for (; k < command.f.size(); ++k) {
      size_t v_idx = fixIndex(command.f[k], v_count);
      uniqueFace_.insert(std::make_pair(std::min(previous_v_idx, v_idx),
                                        std::max(previous_v_idx, v_idx)));

      previous_v_idx = v_idx;
    }

    // Замыкание грани
    size_t v_idx = fixIndex(command.f[0], v_count);
    uniqueFace_.insert(std::make_pair(std::min(previous_v_idx, v_idx), std::max(previous_v_idx, v_idx)));

    if (!command.fShade.empty()) {
      for (auto &f : command.fShade) {
        uniqueFaceShade_.emplace_back(fixIndex(std::get<0>(f), v_count),
                                      fixIndex(std::get<1>(f), v_count),
                                      fixIndex(std::get<2>(f), v_count));
      }
    }
  }
}

Vertex Parser::calculateNormal(Vertex v1, Vertex v2, Vertex v3) {
  Vertex vec1 = {v2.x - v1.x, v2.y - v1.y, v2.z - v1.z};
  Vertex vec2 = {v3.x - v1.x, v3.y - v1.y, v3.z - v1.z};

  Vertex normal = {
      vec1.y * vec2.z - vec1.z * vec2.y,
      vec1.z * vec2.x - vec1.x * vec2.z,
      vec1.x * vec2.y - vec1.y * vec2.x
  };

  float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
  normal.x /= length;
  normal.y /= length;
  normal.z /= length;

  return normal;
}

}  // namespace s21
