#ifndef _OPENGL_HEADERS_
#define _OPENGL_HEADERS_

#ifdef __gnu_linux__
    #include <GL/glut.h>
#elif __APPLE__
    #include <GLUT/glut.h>
#else
    #include <gl/glut.h>
#endif

#endif // _OPENGL_HEADERS_