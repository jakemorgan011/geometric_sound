#pragma once

#include <array>

struct Cube
{
    static constexpr auto xyzArray = std::array{
        // front
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // 0
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // 1
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,   // 2
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // 3

        // top
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 4
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  // 5
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,   // 6
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // 7

        // left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 8
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // 9
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // 10
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // 11

        // right
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, // 12
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  // 13
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,   // 14
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,  // 15

        // back
        -0.5f, -0.5f, 0.5f, 0.7f, 0.7f, 0.7f, // 16
        0.5f, -0.5f, 0.5f, 0.7f, 0.7f, 0.7f,  // 17
        0.5f, 0.5f, 0.5f, 0.7f, 0.7f, 0.7f,   // 18
        -0.5f, 0.5f, 0.5f, 0.7f, 0.7f, 0.7f,  // 19

        // bottom
        -0.5f, -0.5f, 0.5f, 0.3f, 0.3f, 0.3f, // 20
        0.5f, -0.5f, 0.5f, 0.3f, 0.3f, 0.3f,  // 21
        0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f, // 22
        -0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f // 23
    };

    static constexpr auto triangleVertexIndices = std::array{
        // front
        0, 1, 2, // first triangle
        2, 3, 0, // second triangle

        // top
        4, 5, 6, // first triangle
        6, 7, 4, // second triangle

        // left
        8, 9, 10,  // first triangle
        10, 11, 8, // second triangle

        // right
        14, 13, 12, // 12, 13, 14, // first triangle
        12, 15, 14, // 14, 15, 12, // second triangle

        // back
        18, 17, 16, // 16, 17, 18, // first triangle
        16, 19, 18, // 18, 19, 16, // second triangle

        // bottom
        20, 21, 22, // first triangle
        22, 23, 20  // second triangle
    };
};

struct pyramid{
  // tip will have a const pos.
  // see wtf happens when you fuck with the rgb value inside the buffer
  static constexpr auto xyzArray = std::array{
    //front
    -0.5f, -0.5f, -0.5f, 0.f,1.0f,0.f, // 0 (pos + rgb)
    0.5f, -0.5f, -0.5f, 0.f,1.0f,0.f, // 1
    0.f, 0.5f, 0.f, 0.f,1.0f,0.f, // 2 smack center.
    // left
    -0.5f,-0.5f, -0.5f, 1.0f,0.f,0.f, //3
    -0.5f,-0.5f, 0.5f, 1.0f,0.f,0.f, //4
    0.f,0.5f,0.f, 1.0f,0.f,0.f, //5
    // right
    // okay so you have to think rid gridlocked for any of this to work.
    0.5f, -0.5f, -0.5f, 0.f,0.f,1.0f, // 6
    0.5f, -0.5f, 0.5f, 0.f,0.f,1.0f, // 7
    0.f, 0.5f,0.f, 0.f, 0.f,1.0f, //8
    //back
    -0.5f, -0.5f, 0.5f, 0.98f,0.86f,0.86f,//9
    0.5f, -0.5f, 0.5f , 0.98f, 0.86f, 0.86f,//10
    0.f,0.5f,0.f, 0.98f, 0.86f, 0.86f, //11

    //bottom
    -0.5f, -0.5f, 0.5f, 0.77f,0.77f,0.77f, // 12
    0.5f, -0.5f, 0.5f, 0.77f, 0.77f,0.77f, // 13
    0.5f, -0.5f, -0.5f, 0.77f, 0.77f, 0.77f, // 14
    -0.5f, -0.5f, -0.5f, 0.77f, 0.77f, 0.77f // 15
  };

  static constexpr auto triangleVertexIndices = std::array{
    0,1,2, // front
    3,4,5, // left 
    6,7,8, // right
    9,10,11, //back
    
    // bottom is diff
    // needs two triangles
    12,13,14, //first triangle
    14,15,12 // second triangle
  }; 
};
