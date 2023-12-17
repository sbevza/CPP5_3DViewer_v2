#ifndef CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_
#define CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_

#include <cfloat>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace s21 {

constexpr size_t kMaxFacesPerFLine = 200;
constexpr size_t kMaxLineBuffer = 4096;

struct VertexIndex {
  int vIdx;
};

struct Attrib {
  unsigned int numVertices;
  unsigned int numFaces;
  unsigned int numFaceNumVerts;

  float minX, maxX;
  float minY, maxY;
  float minZ, maxZ;

  std::vector<float> vertices;
  std::vector<unsigned int> faces;
};

struct LineInfo {
  size_t pos;
  size_t len;
};

enum class CommandType { EMPTY, V, F };

struct Command {
  float vx, vy, vz;
  int f[kMaxFacesPerFLine];
  size_t numF;
  size_t numFaceNumVerts;
  int f_num_verts[kMaxFacesPerFLine];
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
};
}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_SRC_MODELS_PARSE_H_
