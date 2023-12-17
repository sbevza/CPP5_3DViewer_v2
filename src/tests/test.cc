#include <gtest/gtest.h>

#include "../models/parse.h"

TEST(ParserTest, ParseObjTest) {
  s21::Attrib attrib;
  std::string filename = "./objs/Arisu.obj";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(parser.hasError());
}

TEST(ParserTest, ParseObjTest1) {
  s21::Attrib attrib;
  std::string filename = "./objs/skull.obj";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(parser.hasError());
}

TEST(ParserTest, ParseObjTest2) {
  s21::Attrib attrib;
  std::string filename = "./objs/human.obj";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(parser.hasError());
}

TEST(ParserTest, ParseObjTest3) {
  s21::Attrib attrib;
  std::string filename = "./objs/donut.obj";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(parser.hasError());
}

TEST(ParserTest, ParseObjTest4) {
  s21::Attrib attrib;
  std::string filename = "./objs/cube.obj";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(parser.hasError());
}

TEST(ParserTest, ParseInvalidObjTest1) {
  s21::Attrib attrib;
  std::string filename = "./objs/zero_size.obj";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(!parser.hasError());
}

TEST(ParserTest, ParseInvalidObjTest2) {
  s21::Attrib attrib;
  std::string filename = "./objs/invalid_objs_file.obj";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(!parser.hasError());
}

TEST(ParserTest, ParseInvalidObjTest3) {
  s21::Attrib attrib;
  std::string filename = "";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(!parser.hasError());
}

TEST(ParserTest, ParseInvalidObjTest4) {
  s21::Attrib attrib;
  std::string filename = "objs";

  s21::Parser parser;
  parser.parseObj(attrib, filename);

  ASSERT_FALSE(!parser.hasError());
}