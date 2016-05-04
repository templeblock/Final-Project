#include "matrix_math.h"
#include <GL/glew.h>
#include <math.h>

void eye_proj_mat(GLfloat left, GLfloat right, GLfloat top, GLfloat bottom, GLfloat far, const GLfloat *eye, GLfloat *mat) {
  // I believe this'll only work if everything is behind the xy plane in the -z direction.
  // This is basically an implementation of the multiplication of a matrix generated by glFrustrum and a translation matrix. See https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
  // Everything is translated to camera space first, then the perspective transform is applied.
  // Link to math: https://www.wolframalpha.com/input/?i=[[%282*n%29%2F%28r-l%29,0,%28r%2Bl%29%2F%28r-l%29,0],[0,%282*n%29%2F%28t-b%29,%28t%2Bb%29%2F%28t-b%29,0],[0,0,-%28f%2Bn%29%2F%28f-n%29,-%282*f*n%29%2F%28f-n%29],[0,0,-1,0]]*[[1,0,0,X],[0,1,0,Y],[0,0,1,Z],[0,0,0,1]]

  // Translate the display bounds to eye space.
  right -= eye[0];
  left -= eye[0];
  top -= eye[1];
  bottom -= eye[1];
  far += eye[2]; // Near and far value are positive despite the planes being in the negative z axis.
  GLfloat near = eye[2];

  // row major
  // frustrum * translate
  // I haven't tested this crazy thing yet.
  mat[0] = 2*near/(right-left);
  mat[1] = 0.0;
  mat[2] = (right+left)/(right-left);
  mat[3] = (-2*near*eye[0]-(left+right)*eye[2])/(right-left);

  mat[4] = 0.0;
  mat[5] = 2*near/(top-bottom);
  mat[6] =(bottom+top)/(top-bottom);
  mat[7] =(-2*near*eye[1]-(bottom+top)*eye[2])/(top-bottom);

  mat[8] = 0.0;
  mat[9] = 0.0;
  mat[10] = (-far-near)/(far-near);
  mat[11] = ((far+near)*eye[2]-2*far*near)/(far-near);

  mat[12] = 0.0;
  mat[13] = 0.0;
  mat[14] = -1.0;
  mat[15] = eye[2];

}


void unproject_kinect_coords(int xin, int yin, uint16_t zin, GLfloat *outx, GLfloat *outy, GLfloat *outz) {
// may be necccessary to convert distance to the zin data
z = 0.1236 * Math.tan(zin / 2842.5 + 1.1863);
  // See these pages for info on the kinect depth data
  // https://openkinect.org/wiki/Calibration
  // https://openkinect.org/wiki/Imaging_Information#Depth_Camera
  // https://msdn.microsoft.com/en-us/library/hh973078.aspx
  // TODO implement
  //*outx = xin-320;
  //*outy = yin-240;
  //*outz = zin;
//these constants may not be accurate for this case
//both the data I'm recceiving and this needs to be tested
float minDistance = -10;
float scaleFactor = .0021;
*outx = (xin - 320 / 2) * (z + minDistance) * scaleFactor;
*outy = (yin - 240 / 2) * (z + minDistance) * scaleFactor;
//float theta_offset = 1;
//float pixal_theta = Math.pi-2*theta_offset;
//*outx = -zin/Math.tan(theta_offset+xin*pixal_theta);
//*outy = -zin/Math.tan(theta_offset+yin*pixal_theta);
*outz = z;
//Where
//minDistance = -10
//scaleFactor = .0021.
//These values were found by hand.

}

/*
void unproject_kinect_coords(int xin, int yin, uint16_t zin, GLfloat *outx, GLfloat *outy, GLfloat *outz) {
  // See these pages for info on the kinect depth data
  // https://openkinect.org/wiki/Calibration
  // https://openkinect.org/wiki/Imaging_Information#Depth_Camera
  // https://msdn.microsoft.com/en-us/library/hh973078.aspx
  // TODO implement
  *outx = (-xin+320)/(float)zin*500;
  *outy = (-yin+240)/(float)zin*500;
  *outz = zin;
}
*/


void mat_mult(const GLfloat *m1, const GLfloat *m2, GLfloat *m3, int r1, int c1, int c2) {
  int i, j, k;
  for(i = 0; i < r1; i++) {
    for(j = 0; j < c2; j++) {
      GLfloat sum = 0;
      for(k = 0; k < c1; k++) {
	sum += m1[i*c1+k] * m2[k*c2+j];
      }
      m3[i*c2+j] = sum;
    }
  }
}


void kinect_rot_trans_thingy(const GLfloat *kinectOffset, const GLfloat kinectAngle, const GLfloat *pointOffset, GLfloat *returnPoint) {
  GLfloat hyp = sqrt(pointOffset[0]*pointOffset[0]+pointOffset[2]*pointOffset[2]);
  GLfloat zOff = hyp*sin(kinectAngle);
  GLfloat xOff = hyp*cos(kinectAngle);

  returnPoint[0] = kinectOffset[0] + xOff; // x = kinect_x + hyp*sin(theta)
  returnPoint[1] = kinectOffset[1] + pointOffset[1]; // y = kinect_y + point_y
  returnPoint[2] = zOff+kinectOffset[2]; // z = kinect_z + hyp*cos(theta)
  
}


void scale_rot_trans(GLfloat *m, GLfloat theta, GLfloat phi, GLfloat psi, GLfloat x_scale, GLfloat y_scale, GLfloat z_scale, GLfloat x_trans, GLfloat y_trans, GLfloat z_trans) {
  /* scale
    x_scale 0 0 0
    0 y_scale 0 0
    0 0 z_scale 0
    0 0 0       1
   */

  GLfloat scaling[4*4] = {0};
  scaling[0] = x_scale;
  scaling[5] = y_scale;
  scaling[10] = z_scale;
  scaling[15] = 1;


  /* trans
    0 0 0 x_trans
    0 0 0 y_trans
    0 0 0 z_trans
    0 0 0 1
  */

  GLfloat translation[4*4] = {0};
  scaling[3] = x_trans;
  scaling[7] = y_trans;
  scaling[11] = z_trans;
  scaling[15] = 1;

  /* rotate about z
     cos -sin 0 0
     sin cos  0 0
     0   0    1 0
     0   0    0 1
   */

  GLfloat rotation_Z[4*4] = {0};
  rotation_Z[0] = cos(psi);
  rotation_Z[1] = -sin(psi);
  rotation_Z[4] = -rotation_Z[1];
  rotation_Z[5] = rotation_Z[0];
  rotation_Z[10] = 1;
  rotation_Z[15] = 1;

  /* rotate about x
     1 0   0    0
     0 cos -sin 0
     0 sin cos  0
     0   0    0 1
   */

  GLfloat rotation_X[4*4] = {0};
  rotation_X[0] = 1;
  rotation_X[5] = cos(theta);
  rotation_X[6] = -sin(theta);
  rotation_X[9] = -rotation_X[6];
  rotation_X[10] = rotation_X[5];
  rotation_X[15] = 1;

  /* rotate about y
     cos  0 sin 0
     0    1 0   0
     -sin 0 cos 0
     0   0  0   1
   */

  GLfloat rotation_Y[4*4] = {0};
  rotation_Y[0] = cos(phi);
  rotation_Y[2] = sin(phi);
  rotation_Y[5] = 1;
  rotation_Y[8] = -rotation_Z[2];
  rotation_Y[10] = rotation_Z[0];
  rotation_Y[15] = 1;

  GLfloat tmp[4*4];

  mat_mult(rotation_Z, scaling, m, 4, 4, 4);
  for(int i = 0; i<16; i++) {
    tmp[i] = m[i];
  }
  mat_mult(rotation_X, tmp, m, 4, 4, 4);
  for(int i = 0; i<16; i++) {
    tmp[i] = m[i];
  }
  mat_mult(rotation_Y, tmp, m, 4, 4, 4);
  for(int i = 0; i<16; i++) {
    tmp[i] = m[i];
  }
  mat_mult(translation, tmp, m, 4, 4, 4);
}
