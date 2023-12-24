#ifndef CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_
#define CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_

#include <cfloat>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

namespace s21 {

//constexpr size_t kMaxFacesPerFLine = 200;
constexpr size_t kMaxLineBuffer = 4096;

struct VertexIndex {
  int vIdx;
  int vtIdx;
  int vnIdx;
};

struct Attrib {
  unsigned int numVertices;
//  unsigned int numFaces;
  unsigned int numFaceNumVerts;

  float minX, maxX;
  float minY, maxY;
  float minZ, maxZ;

  std::vector<float> vertices;
  std::vector<float> vertexTexture;
  std::vector<float> vertexNormal;
  std::vector<unsigned int> faces;
  std::vector<unsigned int> facesShade;
  std::vector<size_t> vtIdx;
  std::vector<size_t> vnIdx;
};

struct LineInfo {
  size_t pos;
  size_t len;
};

enum class CommandType { EMPTY, V, F, VT, VN };

struct Command {
  float vx, vy, vz;
  float vtU, vtV;
  float vnX, vnY, vnZ;
  std::vector<int> f;
  std::vector<int> fShade;
  std::vector<int> vtIdx;
  std::vector<int> vnIdx;

  size_t numF;
  size_t numFaceNumVerts;
  CommandType type;
};

class Parser {
 public:
  void parseObj(Attrib &attrib, std::string &obj_filename);
  void attribInit();
  [[nodiscard]] bool hasError() const;

 private:
  bool err_{false};
  std::string filename_;
  Attrib *attrib_{nullptr};

  std::set<std::pair<int, int>> uniqueFace_;
  std::set<std::pair<int, int>> uniqueFaceShade_;

  void commandToAttrib(const std::vector<Command> &commands);
  std::vector<char> getFileData();
  static int isLineEnding(std::vector<char> &ch, size_t i, size_t end_i);
  size_t getLineInfos(std::vector<char> &buf, std::vector<LineInfo> &lineInfos);
  static void skipSpace(const std::string &str, size_t &pos);
  static float parseDouble(const std::string &str, size_t &pos);
  static VertexIndex parseRawTriple(const std::string &str, size_t &pos);
  void parseLine(Command &command, const std::string &line, int &res);
  static size_t fixIndex(int idx, size_t num_v);
  void setAttrib(size_t num_v, size_t num_f, size_t num_faces);
  static void calculateBounds(const std::vector<Command> &commands, float &minX,
                              float &maxX, float &minY, float &maxY,
                              float &minZ, float &maxZ);
  void processVertex(const Command &command, float centerX, float centerY,
                     float centerZ, size_t &v_count);
  void processFace(const Command &command, size_t &f_count, size_t v_count);
  void processTexture(const Command &command);
  void processNormal(const Command &command);
  static void parseVertexCommand(Command &command, const std::string &line,
                                 size_t &pos, int &res);
  static void parseTextureCommand(Command &command, const std::string &line,
                                  size_t &pos, int &res);
  static void parseNormalCommand(Command &command, const std::string &line,
                                 size_t &pos, int &res);
  void parseFaceCommand(Command &command, const std::string &line, size_t &pos,
                        int &res);
};
}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_
