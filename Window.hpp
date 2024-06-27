/**
 * @ Author: Perry Chouteau
 * @ Create Time: 2024-06-08 02:28:02
 * @ Modified by: Perry Chouteau
 * @ Modified time: 2024-06-27 19:26:41
 * @ Description:
 */

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "ogl_headers.h"
#include "timing.h"
#include <string>
#include <iostream>

class Window;

Window &_getWindow(Window *window = nullptr) {
  static Window *_window = window;

  return *_window;
}

void w_close() {
  glutDestroyWindow(glutGetWindow());
  exit(0);
}

class Window {
  public:
    Window(): theta(0.0f), phi(15.0f) {}
    ~Window() = default;

    virtual void createWindow(const char* title);

    virtual const char* getTitle() const {
      return _title.c_str();
    }

    virtual void initGraphics() {
        GLfloat lightAmbient[] = {0.8f,0.8f,0.8f,1.0f};
        GLfloat lightDiffuse[] = {0.9f,0.95f,1.0f,1.0f};

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

        glEnable(GL_LIGHT0);

        glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glShadeModel(GL_SMOOTH);

        setView();
    }

    virtual void setView() {
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(60.0, (double)_width/(double)_height, 1.0, 500.0);
      glMatrixMode(GL_MODELVIEW);
    }

    virtual void Close() {
      TimingData::deinit();
      w_close();
    }

    virtual void setCamera(double posX, double posY, double posZ, double lookX, double lookY, double lookZ) {
      _posX = posX;
      _posY = posY;
      _posZ = posZ;
      _lookX = lookX;
      _lookY = lookY;
      _lookZ = lookZ;
    }

    virtual void display() {
  
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();
      gluLookAt(15.0f, 0, 0,  0, 0, 0,  0, 1.0f, 0);
//      gluLookAt(_posX, _posY, _posZ,  _lookX, _lookY, _lookZ,  0, 5.0f, 0);
      glRotatef(-phi, 0, 0, 1);
      glRotatef(theta, 0, 1, 0);
      glTranslatef(-_posX + 5, -(5.5f), -_posZ);
    }

    virtual void update() {      
      glutPostRedisplay();
    }

    virtual void run() {
      glutMainLoop();
    }

    virtual void key(unsigned char key) {
      if (key == 27) {
        Close();
      }
    }

    virtual void resize(int width, int height) {
      // Avoid the divide by zero.
      if (height <= 0) height = 1;

      // Set the internal variables and update the view
      glViewport(0, 0, width, height);
      setView();
      if (width != glutGet(GLUT_WINDOW_WIDTH) && height != glutGet(GLUT_WINDOW_HEIGHT)) {
        //glutReshapeWindow(width, height);
        std::cout << "Resizing failed !" << std::endl;
      } else {
        _width = width;
        _height = height;
      }
    }

    virtual void mouse(int button, int state, int x, int y) {
      // Set the position
      last_x = x;
      last_y = y;
    }

    virtual void mouseDrag(int x, int y) {
       // Update the camera
      theta += (x - last_x)*0.25f;
      phi += (y - last_y)*0.25f;

      // Keep it in bounds
      if (phi < -20.0f) phi = -20.0f;
      else if (phi > 80.0f) phi = 80.0f;

      // Remember the position
      last_x = x;
      last_y = y;
    }

    virtual void renderText(float x, float y, const char *text, void* font=NULL) {
      glDisable(GL_DEPTH_TEST);

      // Temporarily set up the view in orthographic projection.
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glOrtho(0.0, (double)_width, 0.0, (double)_height, -1.0, 1.0);

      // Move to modelview mode.
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();

      // Ensure we have a font
      if (font == NULL) {
          font = GLUT_BITMAP_HELVETICA_10;
      }

      // Loop through characters displaying them.
      size_t len = strlen(text);

      glRasterPos2f(x, y);
      for (const char *letter = text; letter < text+len; letter++) {

          // If we meet a newline, then move down by the line-height
          // TODO: Make the line-height a function of the font
          if (*letter == '\n') {
              y -= 12.0f;
              glRasterPos2f(x, y);
          }
          glutBitmapCharacter(font, *letter);
      }

      // Pop the matrices to return to how we were before.
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);

      glEnable(GL_DEPTH_TEST);
    }

  private:
    std::string _title;

    int _width;
    int _height;

    /** Holds the camera angle. */
    float theta;
    /** Holds the camera elevation. */
    float phi;

    //camera
    double _posX, _posY, _posZ;
    double _lookX, _lookY, _lookZ;
  public:
    /** Holds the position of the mouse at the last frame of a drag. */
    int last_x, last_y;


};

void _resize(int width, int height) {
  Window &window = _getWindow();

  window.resize(width, height);
}

void _display() {
  Window &window = _getWindow();

  window.display();
  
  // Update the displayed content.
  glFlush();
  glutSwapBuffers();

}

void _update() {
  TimingData::get().update();
  Window &window = _getWindow();
  
  window.update();
}

void _keyboard(unsigned char key, int x, int y) {
  Window &window = _getWindow();
  window.key(key);
}

void _mouse(int button, int state, int x, int y) {
  Window &window = _getWindow();
  window.mouse(button, state, x, y);
}

void _motion(int x, int y) {
  Window &window = _getWindow();
  window.mouseDrag(x, y);
}

void Window::createWindow(const char* title) {
  _getWindow(this);
  
  TimingData::init();

  int ac = 0;
  glutInit(&ac, nullptr);
  // Set up GLUT and the timers
  
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  //setup screen
  _width = glutGet(GLUT_SCREEN_WIDTH) / 2;
  _height = glutGet(GLUT_SCREEN_HEIGHT) / 2;

  glutInitWindowPosition(_width / 2, _height / 2);
  glutInitWindowSize(_width, _height);
  _title = title;
  glutCreateWindow(title);

  glutReshapeFunc(_resize);
  glutIdleFunc(_update);
  glutDisplayFunc(_display);
  glutWMCloseFunc(w_close);

  glutKeyboardFunc(_keyboard);
  glutMouseFunc(_mouse);
  glutMotionFunc(_motion);
  
  this->initGraphics();
}

#endif // WINDOW_HPP