#include "texture.h"

#include <assert.h>
#include <iostream>
#include <algorithm>

using namespace std;

namespace CMU462 {

inline void uint8_to_float( float dst[4], unsigned char* src ) {
  uint8_t* src_uint8 = (uint8_t *)src;
  dst[0] = src_uint8[0] / 255.f;
  dst[1] = src_uint8[1] / 255.f;
  dst[2] = src_uint8[2] / 255.f;
  dst[3] = src_uint8[3] / 255.f;
}

inline void float_to_uint8( unsigned char* dst, float src[4] ) {
  uint8_t* dst_uint8 = (uint8_t *)dst;
  dst_uint8[0] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[0])));
  dst_uint8[1] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[1])));
  dst_uint8[2] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[2])));
  dst_uint8[3] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[3])));
}

void Sampler2DImp::generate_mips(Texture& tex, int startLevel) {

  // NOTE(sky):
  // The starter code allocates the mip levels and generates a level
  // map simply fills each level with a color that differs from its
  // neighbours'. The reference solution uses trilinear filtering
  // and it will only work when you have mipmaps.

  // Task 7: Implement this

  // check start level
  if ( startLevel >= tex.mipmap.size() ) {
    std::cerr << "Invalid start level";
  }

  // allocate sublevels
  int baseWidth  = tex.mipmap[startLevel].width;
  int baseHeight = tex.mipmap[startLevel].height;
  int numSubLevels = (int)(log2f( (float)max(baseWidth, baseHeight)));

  numSubLevels = min(numSubLevels, kMaxMipLevels - startLevel - 1);
  tex.mipmap.resize(startLevel + numSubLevels + 1);

  int width  = baseWidth;
  int height = baseHeight;
  for (int i = 1; i <= numSubLevels; i++) {

    MipLevel& level = tex.mipmap[startLevel + i];

    // handle odd size texture by rounding down
    width  = max( 1, width  / 2); assert(width  > 0);
    height = max( 1, height / 2); assert(height > 0);

    level.width = width;
    level.height = height;
    level.texels = vector<unsigned char>(4 * width * height);

  }

  // fill all 0 sub levels with interchanging colors
  Color colors[3] = { Color(1,0,0,1), Color(0,1,0,1), Color(0,0,1,1) };
  for(size_t i = 1; i < tex.mipmap.size(); ++i) {

    /*
    Color c = colors[i % 3];
    MipLevel& mip = tex.mipmap[i];

    for(size_t idx = 0; idx < 4 * mip.width * mip.height; idx += 4) {
      float_to_uint8( &mip.texels[i], &c.r );
    }
    */

    for (size_t x = 0; x < tex.mipmap[i].width - 1; x++) {
      for(size_t y = 0; y < tex.mipmap[i].height - 1; y++) {
        Color c = sample_bilinear(tex, 2 * x, 2 * y, i - 1);

        tex.mipmap[i].texels[4 * (x + y * tex.mipmap[i].width)    ] = c.r;
        tex.mipmap[i].texels[4 * (x + y * tex.mipmap[i].width) + 1] = c.g;
        tex.mipmap[i].texels[4 * (x + y * tex.mipmap[i].width) + 1] = c.b;
        tex.mipmap[i].texels[4 * (x + y * tex.mipmap[i].width) + 1] = c.a;
      }
    }
  }

}

Color Sampler2DImp::sample_nearest(Texture& tex,
                                   float u, float v,
                                   int level) {

  // Task 6: Implement nearest neighbour interpolation

  int int_u = (int) floor(u * tex.mipmap[level].width);
  int int_v = (int) floor(v * tex.mipmap[level].height);

  float red =   (float)tex.mipmap[level].texels[4 * (int_u + int_v * tex.mipmap[level].width)    ];
  float green = (float)tex.mipmap[level].texels[4 * (int_u + int_v * tex.mipmap[level].width) + 1];
  float blue =  (float)tex.mipmap[level].texels[4 * (int_u + int_v * tex.mipmap[level].width) + 2];
  float alpha = (float)tex.mipmap[level].texels[4 * (int_u + int_v * tex.mipmap[level].width) + 3];

  red /= 255;
  green /= 255;
  blue /= 255;
  alpha /= 255;

  // return magenta for invalid level
  return Color(red, green, blue, alpha);

}

Color Sampler2DImp::sample_bilinear(Texture& tex,
                                    float u, float v,
                                    int level) {

  // Task 6: Implement bilinear filtering
  //Get 4 nearest pixels in texture map and average
  if(level < 0) {
    level = 0;
  }

  int int_u = (int) floor(u * tex.mipmap[level].width);
  int int_v = (int) floor(v * tex.mipmap[level].height);

  float r0 = (float)tex.mipmap[level].texels[4 * (int_u + int_v * tex.mipmap[level].width)    ];
  float g0 = (float)tex.mipmap[level].texels[4 * (int_u + int_v * tex.mipmap[level].width) + 1];
  float b0 = (float)tex.mipmap[level].texels[4 * (int_u + int_v * tex.mipmap[level].width) + 2];
  float a0 = (float)tex.mipmap[level].texels[4 * (int_u + int_v * tex.mipmap[level].width) + 3];

  float r1 = (float)tex.mipmap[level].texels[4 * ((int_u + 1) + int_v * tex.mipmap[level].width)    ];
  float g1 = (float)tex.mipmap[level].texels[4 * ((int_u + 1) + int_v * tex.mipmap[level].width) + 1];
  float b1 = (float)tex.mipmap[level].texels[4 * ((int_u + 1) + int_v * tex.mipmap[level].width) + 2];
  float a1 = (float)tex.mipmap[level].texels[4 * ((int_u + 1) + int_v * tex.mipmap[level].width) + 3];

  float r2 = (float)tex.mipmap[level].texels[4 * (int_u + (int_v + 1) * tex.mipmap[level].width)    ];
  float g2 = (float)tex.mipmap[level].texels[4 * (int_u + (int_v + 1) * tex.mipmap[level].width) + 1];
  float b2 = (float)tex.mipmap[level].texels[4 * (int_u + (int_v + 1) * tex.mipmap[level].width) + 2];
  float a2 = (float)tex.mipmap[level].texels[4 * (int_u + (int_v + 1) * tex.mipmap[level].width) + 3];

  float r3 = (float)tex.mipmap[level].texels[4 * ((int_u + 1) + (int_v + 1) * tex.mipmap[level].width)    ];
  float g3 = (float)tex.mipmap[level].texels[4 * ((int_u + 1) + (int_v + 1) * tex.mipmap[level].width) + 1];
  float b3 = (float)tex.mipmap[level].texels[4 * ((int_u + 1) + (int_v + 1) * tex.mipmap[level].width) + 2];
  float a3 = (float)tex.mipmap[level].texels[4 * ((int_u + 1) + (int_v + 1) * tex.mipmap[level].width) + 3];

  float red = (r0 + r1 + r2 + r3) / (4 * 255);
  float green = (g0 + g1 + g2 + g3) / (4 * 255);
  float blue = (b0 + b1 + b2 + b3) / (4 * 255);
  float alpha = (a0 + a1 + a2 + a3) / (4 * 255);

  return Color(red, green, blue, alpha);

}

Color Sampler2DImp::sample_trilinear(Texture& tex,
                                     float u, float v,
                                     float u_scale, float v_scale) {

  // Task 7: Implement trilinear filtering

  float dudx = u * u_scale;
  //cout<<"dudx = " << dudx << "\n";
  float dvdy = v * v_scale;
  //cout<<"dvdy = " << dvdy << "\n";
  float maxd = (dvdy < dudx) ? dudx : dvdy;
  int d = (int) log2(maxd);

  cout << "d = " << d << "\n";

  Color c0 = sample_bilinear(tex, dudx, dvdy, d);
  Color c1 = sample_bilinear(tex, dudx, dvdy, d);

  float red   = (c0.r + c1.r) / 2;
  float green = (c0.r + c1.r) / 2;
  float blue  = (c0.r + c1.r) / 2;
  float alpha = (c0.r + c1.r) / 2;

  return Color(red, green, blue, alpha);

}

} // namespace CMU462
