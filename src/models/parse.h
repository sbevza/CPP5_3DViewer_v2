#ifndef CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_
#define CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

namespace s21 {

struct Bounds {
  float minX = std::numeric_limits<float>::max();
  float maxX = -std::numeric_limits<float>::max();
  float minY = std::numeric_limits<float>::max();
  float maxY = -std::numeric_limits<float>::max();
  float minZ = std::numeric_limits<float>::max();
  float maxZ = -std::numeric_limits<float>::max();
};

struct VertexIndex {
  int vIdx;
  int vtIdx;
  int vnIdx;
};

struct Vertex {
  float x, y, z;

  Vertex(float xCoord, float yCoord, float zCoord)
      : x(xCoord), y(yCoord), z(zCoord) {}
};

struct Attrib {
  float minX, maxX;
  float minY, maxY;
  float minZ, maxZ;

  std::vector<float> vertices;
  std::vector<float> verticesShade;
  std::vector<float> vertexTexture;
  std::vector<float> vertexTextureShade;
  std::vector<float> vertexNormal;
  std::vector<float> vertexNormalShade;
  std::vector<unsigned int> faces;
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
  std::vector<std::tuple<int, int, int>> fShade;
  CommandType type;
};

class Parser {
 public:
  void parseObj(Attrib &attrib, std::string &obj_filename);
  void attribInit();
  [[nodiscard]] bool hasError() const;

 private:
  bool err_{false};
  Attrib *attrib_{nullptr};
  std::set<std::pair<int, int>> uniqueFace_;
  std::vector<std::tuple<int, int, int>> uniqueFaceShade_;

  void commandToAttrib(const std::vector<Command> &commands);
  static std::vector<char> getFileData(const std::string &filename);
  static int isLineEnding(const std::vector<char> &ch, size_t i, size_t end_i);
  static std::vector<LineInfo> getLineInfos(const std::vector<char> &buf);
  static void processLine(std::vector<Command> &commands,
                          const std::vector<char> &buf,
                          const LineInfo &lineInfo, size_t &numV);
  static VertexIndex parseRawTriple(const std::string &str, size_t &pos);
  static bool parseLine(Command &command, const std::string &line);
  static size_t fixIndex(int idx, size_t num_v);
  void setAttrib(size_t num_v);
  static void calculateBounds(const std::vector<Command> &commands,
                              Bounds &bounds);
  void processVertex(const Command &command, float centerX, float centerY,
                     float centerZ, size_t &v_count);
  void processFace(const Command &command, size_t v_count);
  void processTexture(const Command &command);
  void processNormal(const Command &command);
  static void parseFaceCommand(Command &command, std::istringstream &iss);
  static Vertex calculateNormal(Vertex v1, Vertex v2, Vertex v3);
  void recalculateNormals();
  void calculateShadeModel();
  void updateAttribPositions(float centerX, float centerY, float centerZ,
                             Bounds &bounds);
};
}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_
