#include "software_renderer.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include "triangulation.h"

using namespace std;

namespace CMU462 {


// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {

  // set top level transformation
  transformation = canvas_to_screen;

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
  }

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y++;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y++;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y--;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y--;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  // resolve and send to render target
  resolve();

}

void SoftwareRendererImp::set_sample_rate( size_t sample_rate ) {

  // Task 4:
  // You may want to modify this for supersampling support
  this->sample_rate = sample_rate;

}

void SoftwareRendererImp::set_render_target( unsigned char* render_target,
                                             size_t width, size_t height ) {

  // Task 4:
  // You may want to modify this for supersampling support
  this->render_target = render_target;
  this->target_w = width;
  this->target_h = height;

  this->w = width * this->sample_rate;
  this->h = height * this->sample_rate;
  this->sample_buffer.resize(4 * this->w * this->h);
}

void SoftwareRendererImp::draw_element( SVGElement* element ) {

  // Task 5 (part 1):
  // Modify this to implement the transformation stack

  Matrix3x3 temp_trans = transformation;
  transformation = transformation * (element->transform);

  switch(element->type) {
    case POINT:
      draw_point(static_cast<Point&>(*element));
      break;
    case LINE:
      draw_line(static_cast<Line&>(*element));
      break;
    case POLYLINE:
      draw_polyline(static_cast<Polyline&>(*element));
      break;
    case RECT:
      draw_rect(static_cast<Rect&>(*element));
      break;
    case POLYGON:
      draw_polygon(static_cast<Polygon&>(*element));
      break;
    case ELLIPSE:
      draw_ellipse(static_cast<Ellipse&>(*element));
      break;
    case IMAGE:
      draw_image(static_cast<Image&>(*element));
      break;
    case GROUP:
      draw_group(static_cast<Group&>(*element));
      break;
    default:
      break;
  }
  transformation = temp_trans;
}


// Primitive Drawing //

void SoftwareRendererImp::draw_point( Point& point ) {

  Vector2D p = transform(point.position);
  rasterize_point( p.x, p.y, point.style.fillColor );

}

void SoftwareRendererImp::draw_line( Line& line ) {

  Vector2D p0 = transform(line.from);
  Vector2D p1 = transform(line.to);
  rasterize_line( p0.x, p0.y, p1.x, p1.y, line.style.strokeColor );

}

void SoftwareRendererImp::draw_polyline( Polyline& polyline ) {

  Color c = polyline.style.strokeColor;

  if( c.a != 0 ) {
    int nPoints = polyline.points.size();
    for( int i = 0; i < nPoints - 1; i++ ) {
      Vector2D p0 = transform(polyline.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polyline.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_rect( Rect& rect ) {

  Color c;

  // draw as two triangles
  float x = rect.position.x;
  float y = rect.position.y;
  float w = rect.dimension.x;
  float h = rect.dimension.y;

  Vector2D p0 = transform(Vector2D(   x   ,   y   ));
  Vector2D p1 = transform(Vector2D( x + w ,   y   ));
  Vector2D p2 = transform(Vector2D(   x   , y + h ));
  Vector2D p3 = transform(Vector2D( x + w , y + h ));

  // draw fill
  c = rect.style.fillColor;
  if (c.a != 0 ) {
    rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  }

  // draw outline
  c = rect.style.strokeColor;
  if( c.a != 0 ) {
    rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    rasterize_line( p1.x, p1.y, p3.x, p3.y, c );
    rasterize_line( p3.x, p3.y, p2.x, p2.y, c );
    rasterize_line( p2.x, p2.y, p0.x, p0.y, c );
  }

}

void SoftwareRendererImp::draw_polygon( Polygon& polygon ) {

  Color c;

  // draw fill
  c = polygon.style.fillColor;
  if( c.a != 0 ) {

    // triangulate
    vector<Vector2D> triangles;
    triangulate( polygon, triangles );

    // draw as triangles
    for (size_t i = 0; i < triangles.size(); i += 3) {
      Vector2D p0 = transform(triangles[i + 0]);
      Vector2D p1 = transform(triangles[i + 1]);
      Vector2D p2 = transform(triangles[i + 2]);
      rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    }
  }

  // draw outline
  c = polygon.style.strokeColor;
  if( c.a != 0 ) {
    int nPoints = polygon.points.size();
    for( int i = 0; i < nPoints; i++ ) {
      Vector2D p0 = transform(polygon.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polygon.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_ellipse( Ellipse& ellipse ) {

  // Extra credit

}

void SoftwareRendererImp::draw_image( Image& image ) {

  Vector2D p0 = transform(image.position);
  Vector2D p1 = transform(image.position + image.dimension);

  rasterize_image( p0.x, p0.y, p1.x, p1.y, image.tex );
}

void SoftwareRendererImp::draw_group( Group& group ) {

  for ( size_t i = 0; i < group.elements.size(); ++i ) {
    draw_element(group.elements[i]);
  }

}

// Rasterization //

// The input arguments in the rasterization functions
// below are all defined in screen space coordinates

void SoftwareRendererImp::rasterize_point( float x, float y, Color color ) {

  // fill in the nearest pixel
  int sx = (int) floor(x);
  int sy = (int) floor(y);

  // check bounds
  if ( sx < 0 || sx >= target_w ) return;
  if ( sy < 0 || sy >= target_h ) return;

  // fill sample - NOT doing alpha blending!

  sample_buffer[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
  sample_buffer[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
  sample_buffer[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
  sample_buffer[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);

  /*
  float Er = color.r, Eg = color.g, Eb = color.b;
  float Ea = color.a;
  float Cr = (float)(sample_buffer[4 * (sx + sy * target_w)    ] / 255);
  float Cg = (float)(sample_buffer[4 * (sx + sy * target_w) + 1] / 255);
  float Cb = (float)(sample_buffer[4 * (sx + sy * target_w) + 2] / 255);
  float Ca = (float)(sample_buffer[4 * (sx + sy * target_w) + 3] / 255);

  float a = 1 - (1 - Ea) * (1 - Ca);
  float r = (1 - Ea) * Cr + Er;
  float g = (1 - Ea) * Cg + Eg;
  float b = (1 - Ea) * Cb + Eb;

  sample_buffer[4 * (sx + sy * target_w)    ] = (uint8_t) (r * 255);
  sample_buffer[4 * (sx + sy * target_w) + 1] = (uint8_t) (g * 255);
  sample_buffer[4 * (sx + sy * target_w) + 2] = (uint8_t) (b * 255);
  sample_buffer[4 * (sx + sy * target_w) + 3] = (uint8_t) (a * 255);
  */
}

void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {

  // Task 2:
  // Implement line rasterization

  float m = (y1 - y0) / (x1 - x0);
  bool large_slope = (m < -1 || 1 < m);
  if(!large_slope) {
    if(x1 < x0) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }
    float x;
    float e = 0;
    float y = y0;
    float ystep = (y0 < y1) ? 1 : -1;
    bool neg = (y1 < y0);

    //printf("Small: slope = %f\n", m);
    for(x = x0; x < x1; x++) {
      rasterize_point(x, y, color);
      if(!neg) { //positive slope
        if(e + m < 0.5) {
          e = e + m;
        } else {
          y = y + ystep;
          e = e + m - ystep;
        }
      } else { //negative slope
        if(-0.5 < e + m) {
          e = e + m;
        } else {
          y = y + ystep;
          e = e + m - ystep;
        }
      }
    }
  } else { //large_slope
    m = (x1 - x0) / (y1 - y0);
    if(y1 < y0) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }
    float x = x0;
    float y;
    float e = 0;
    float xstep = (x0 < x1) ? 1 : -1;
    bool neg = (x1 < x0);

    for(y = y0; y < y1; y++) {
      rasterize_point(x, y, color);
      if(!neg) { //positive slope
        if(e + m < 0.5) {
          e = e + m;
        } else {
          x = x + xstep;
          e = e + m - xstep;
        }
      } else { //negative slope
        if(-0.5 < e + m) {
          e = e + m;
        } else {
          x = x + xstep;
          e = e + m - xstep;
        }
      }
    }
  }
}

void SoftwareRendererImp::rasterize_triangle( float x0, float y0,
                                              float x1, float y1,
                                              float x2, float y2,
                                              Color color ) {
  // Task 3:
  // Implement triangle rasterization

  //Draw edges of triangle
  rasterize_line(x0, y0, x1, y1, color);
  rasterize_line(x1, y1, x2, y2, color);
  rasterize_line(x2, y2, x0, y0, color);

  //Fill in triangle
  float low_x = (x0 < x1) ? ((x0 < x2) ? x0 : x2) : ((x1 < x2) ? x1 : x2);
  float low_y = (y0 < y1) ? ((y0 < y2) ? y0 : y2) : ((y1 < y2) ? y1 : y2);
  float hi_x = (x0 > x1) ? ((x0 > x2) ? x0 : x2) : ((x1 > x2) ? x1 : x2);
  float hi_y = (y0 > y1) ? ((y0 > y2) ? y0 : y2) : ((y1 > y2) ? y1 : y2);

  float x, y;
  for(x = low_x; x < hi_x; x += 0.5) {
    for(y = low_y; y < hi_y; y += 0.5) {

      bool b0 = ((x - x1) * (y0 - y1) - (x0 - x1) * (y - y1)) < 0.0f;
      bool b1 = ((x - x2) * (y1 - y2) - (x1 - x2) * (y - y2)) < 0.0f;
      bool b2 = ((x - x0) * (y2 - y0) - (x2 - x0) * (y - y0)) < 0.0f;

      if((b0 == b1) && (b1 == b2)) {
        rasterize_point(x, y, color);
      }
    }
  }
}

void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
  // Task 6:
  // Implement image rasterization

  Sampler2D* sampler = this->sampler;

  float x, y;
  float u, v;
  Color color;
  for(x = x0; x < x1; x++) {
    for(y = y0; y < y1; y++) {
      u = (x - x0) / (x1 - x0);
      v = (y - y0) / (y1 - y0);
      //color = sampler->sample_nearest(tex, u, v, 0);
      color = sampler->sample_bilinear(tex, u, v, 0);
      rasterize_point(x, y, color);
    }
  }

}

// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

  // Task 4:
  // Implement supersampling
  // You may also need to modify other functions marked with "Task 4".

  int x, y;
  int xbuf, ybuf;
  int xsample, ysample;
  int xidx, yidx;
  float sum_r, sum_g, sum_b, sum_a;
  for(x = 0; x < this->w; x += sample_rate) {
    for(y = 0; y < this->h; y += sample_rate) {
      sum_r = 0;
      sum_g = 0;
      sum_b = 0;
      sum_a = 0;
      for(xbuf = 0; xbuf < sample_rate; xbuf++) {
        for(ybuf = 0; ybuf < sample_rate; ybuf++) {
          xsample = x + xbuf;
          ysample = y + ybuf;
          sum_r += sample_buffer[4 * (xsample + (ysample * target_w))    ];
          sum_g += sample_buffer[4 * (xsample + (ysample * target_w)) + 1];
          sum_b += sample_buffer[4 * (xsample + (ysample * target_w)) + 2];
          sum_a += sample_buffer[4 * (xsample + (ysample * target_w)) + 3];
        }
      }
      xidx = floor(x / sample_rate);
      yidx = floor(y / sample_rate);
      render_target[4 * (xidx + yidx * target_w)    ] = sum_r / pow(sample_rate, 2);
      render_target[4 * (xidx + yidx * target_w) + 1] = sum_g / pow(sample_rate, 2);
      render_target[4 * (xidx + yidx * target_w) + 2] = sum_b / pow(sample_rate, 2);
      render_target[4 * (xidx + yidx * target_w) + 3] = sum_a / pow(sample_rate, 2);
    }
  }
  return;

}


} // namespace CMU462
