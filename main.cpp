/*
 * The main entry point for all demos.
 *
 * Part of the Cyclone physics system.
 *
 * Copyright (c) Icosagon 2003. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

#include "Window.hpp"
#include <cyclone.h>
#include "timing.h"

#include "Canon.hpp"
#include "Box.hpp"

static void drawBoat()
{
    //Draw Deck
    //Draw Bow



    // Left Hull
//    glPushMatrix();
//    glTranslatef(0, 0, -1.0f);
//    glScalef(2.0f, 0.4f, 0.4f);
//    glutSolidCube(1.0f);
//    glPopMatrix();

    // Right Hull
//    glPushMatrix();
//    glTranslatef(0, 0, 1.0f);
//    glScalef(2.0f, 0.4f, 0.4f);
//    glutSolidCube(1.0f);
//    glPopMatrix();

    // Deck
    glPushMatrix();
    glColor3f(0.75,0.45,0.1);
    glTranslatef(0, 0.2f, 0);
    glScalef(2.0f, 0.25f, 2.0f);
    glutSolidCube(1.0f);
    glColor3f(0,0,0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.75,0.45,0.1);
    glTranslatef(-1, 0.3f, 0);
    glScalef(1.5f, 0.25f, 1.5f);
    glutSolidCube(1.0f);
    glColor3f(0,0,0);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.75,0.45,0.1);
    glTranslatef(1, 0.3f, 0);
    glScalef(1.5f, 0.25f, 1.5f);
    glutSolidCube(1.0f);
    glColor3f(0,0,0);
    glPopMatrix();


    // Mast
    glPushMatrix();
    glColor3f(0.65,0.35,0.1);
    glTranslatef(0, 1.2f, 0);
    glScalef(0.1f, 2.0f, 0.1f);
    glutSolidCube(1.0f);
    glColor3f(0, 0, 0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.65,0.35,0.1);
    glTranslatef(0, 2.3f, 0);
    glScalef(0.1f, 0.1f, 2.f);
    glutSolidCube(1.0f);
    glColor3f(0, 0, 0);
    glPopMatrix();

    // Sail

    glPushMatrix();
    glColor3f(1, 1, 1);
    glTranslatef(-0.1, 1.8f, 0);
    glScalef(0.1f, 1.f, 2.f);
    glutSolidCube(1.0f);
    glColor3f(0, 0, 0);
    glPopMatrix();

    glPopMatrix();
}

class Game : public Window {
    cyclone::RigidBody sailboat;
//    cyclone::Aero sail;
    cyclone::Buoyancy buoyancy;
    cyclone::Vector3 windspeed;
    Canon canon;

    Box boxes[3];

    unsigned startTime;
    int score = 0;

    //force
    cyclone::ForceRegistry registry;
    
    //colision
    
    /** Holds the maximum number of contacts. */
    const static unsigned maxContacts = 256;

    /** Holds the array of contacts. */
    cyclone::Contact contacts[maxContacts];

    /** Holds the collision data structure for collision detection. */
    cyclone::CollisionData cData;

    /** Holds the contact resolver. */
    cyclone::ContactResolver resolver;


public:
    /** Creates a new demo object. */
    Game(): Window(), windspeed(0,0,0), buoyancy(cyclone::Vector3(0.0f, 0.5f, 0.0f), 1.0f, 3.0f, 1.4f), resolver(maxContacts*8) {
        // Set up the boat's rigid body.
        sailboat.setPosition(0, 0, 0);
        sailboat.setOrientation(1,0,0,0);
        sailboat.setVelocity(0,0,0);
        sailboat.setRotation(0,0,0);

        sailboat.setMass(400.f);
        cyclone::Matrix3 it;
        it.setBlockInertiaTensor(cyclone::Vector3(2,1,1), 100.0f);
        sailboat.setInertiaTensor(it);

        sailboat.setDamping(0.8f, 0.8f);

        sailboat.calculateDerivedData();

        sailboat.setAwake();
        sailboat.setCanSleep(false);
        
        registry.add(&sailboat, &buoyancy);

        sailboat.addTorque(cyclone::Vector3(0, 1, 0));

        for (int i = 0; i< 3; i++) {
            registry.add(boxes[i].body, &buoyancy);
        }

        cData.contactArray = contacts;

        //calcul new vector of acceleration after rotation.
        //        sailboat.setAcceleration(sailboat.getTransform().transformDirection(cyclone::Vector3(0, 0, -1)));

    }

    ~Game() = default;


    virtual void display() {
        cyclone::Vector3 pos = sailboat.getPosition();
        cyclone::Vector3 offset(4.0f, 0, 0);
        offset = sailboat.getTransform().transformDirection(offset);
        Window::setCamera(pos.x+offset.x, pos.y+5.0f, pos.z+offset.z, pos.x, pos.y, pos.z);
        Window::display();

        glColor3f(0.3f,0.3f,0.8f);
        int bx = int(pos.x);
        int bz = int(pos.z);
        glBegin(GL_QUADS);
        cyclone::Random rnd;
        
        // Draw a tiled floor
        for (int x = -50; x <= 50; x++) {
            for (int z = -50; z <= 50; z++) {
                cyclone::real v = 0;//cyclone::Random().randomReal(-0.05f, 0.05f);

                glVertex3f(bx+x-0.45f, v, bz+z-0.45f);
                glVertex3f(bx+x-0.45f, v, bz+z+0.45f);
                glVertex3f(bx+x+0.45f, v, bz+z+0.45f);
                glVertex3f(bx+x+0.45f, v, bz+z-0.45f);
            }
        }
        glEnd();

        // Set the transform matrix for the aircraft
        cyclone::Matrix4 transform = sailboat.getTransform();
        GLfloat gl_transform[16];
        transform.fillGLArray(gl_transform);
        glPushMatrix();
        glMultMatrixf(gl_transform);

        // Draw the boat
        drawBoat();

        canon.render();
        
        for (int i = 0; i < 3; i++) {
            if (boxes[i].used == true) {
                boxes[i].render();
            }
        }

        char buffer[256];
        sprintf( buffer, "Score %i", score);
        glColor3f(0,0,0);
        renderText(20.0f, 500.0f, buffer);

        sprintf( buffer, "Speed %0.f", sailboat.getVelocity().magnitude());
        renderText(20.0f, 480.0f, buffer);

    }

    virtual void update() {
        // Find the duration of the last frame in seconds
        float duration = (float)TimingData::get().lastFrameDuration * 0.001f;
        if (duration <= 0.0f) return;

        // Start with no forces or acceleration.
        sailboat.clearAccumulators();
        
        for (int i = 0; i < 3; i++) {
            boxes[i].body->clearAccumulators();
        }


        // Add the forces acting on the boat.
        registry.updateForces(duration);

        // Update the boat's physics.
        sailboat.integrate(duration);

        // Change the wind speed.
        windspeed = windspeed * 0.9f; //+ r.randomXZVector(1.0f);
        //setup cannon
        canon.setOrientation(sailboat.getOrientation(), sailboat.getPosition());
        canon.update(duration);

          // Perform the contact generation
        generateContacts();

    // Resolve detected contacts
        resolver.resolveContacts(
            cData.contactArray,
            cData.contactCount,
            duration
            );
    
        Window::update();
    }

    virtual const char* getTitle()
    {
        return "Cyclone > Sail Boat Demo";
    }

    virtual void key(unsigned char key)
    {
        cyclone::Quaternion q = sailboat.getOrientation();
        q.normalise();

        switch(key) {
            case 'z': case 'Z':
    //            sail_control -= 0.1f;
        
                sailboat.getOrientation();
                sailboat.addVelocity((q * cyclone::Vector3(-0.1f, 0.0f, 0.0f)));
                std::cout << "Z put the sail down" << std::endl;
                break;

            case 's': case 'S':
                sailboat.getOrientation();
                sailboat.addVelocity((q * cyclone::Vector3(0.1,0,0)));
                std::cout << "S put the sail up to slow" << std::endl;
                break;

            case 'q': case 'Q':
                sailboat.addRotation(cyclone::Vector3(0,0.1,0));
                std::cout << "Q turn the bar" << std::endl;
                break;

            case 'd': case 'D':
                sailboat.addRotation(cyclone::Vector3(0,-0.1,0));
                std::cout << "D  turn the bar" << std::endl;
                break;
            case '1': case '&':
                canon.setBulletType(BulletType::DIRECT);
                break;
            case '2': case 195: //195=é
                canon.setBulletType(BulletType::DIPPING);
                break;

            default:
                Window::key(key);
            std::cout << (int)key << std::endl;
        }

        // Make sure the controls are in range

        // Update the control surfaces
        //sail.setControl(sail_control);
    }
    enum MouseState {
        Unused = 0,
        OnDragRelease = 1,
        Pressed = 2,
        ClickRelease = 3,
    };

    virtual void mouse(int button, int state, int x, int y) {
        static MouseState mState = Unused;
        static int begX, begY;
        const int DRAG_THRESHOLD = 5;
        static unsigned startTime = 0;
      // Set the position
      
      if (state == GLUT_DOWN) {
        begX = x; begY = y;
        startTime = TimingData::get().lastFrameTimestamp;
        state = Pressed;
      } else if (state == GLUT_UP) {
        printf("%i\n" , std::abs(begX - x));
        printf("%i\n" , std::abs(begY - y));
        mState = (std::abs(begX - x) < DRAG_THRESHOLD && 
                    std::abs(begY - y) < DRAG_THRESHOLD //&& 
                    /*(startTime+5000) > TimingData::get().lastFrameTimestamp*/)
            ? ClickRelease: OnDragRelease;
      }
        printf("state %i\n", mState == ClickRelease ? 1 : 0);
      if (mState == ClickRelease) {
        canon.fire();
        mState = Unused;
      }
      Window::mouse(button, state, x, y);
    }


    void generateContacts() {
        cData.reset(maxContacts); //todo
        cData.friction = (cyclone::real)0.9;
        cData.restitution = (cyclone::real)0.1;
        cData.tolerance = (cyclone::real)0.1;

        for (Box *box = boxes; box < boxes+3; box++) {

            // Check for collisions with each shot
            for (Bullet *bullet = canon.mag; bullet < canon.mag+canon.magSize; bullet++)
            {
                if (bullet->type != UNUSED)
                {
                    if (!cData.hasMoreContacts()) return;

                    // When we get a collision, remove the shot
                    if (cyclone::CollisionDetector::boxAndSphere(*box, *bullet, &cData))
                    {
                        std::cout << "hit" << std::endl;
                        bullet->type = UNUSED;
                        score += 1;
                    }
                }
            }
        }
    }

};

/**
 * The main entry point. We pass arguments onto GLUT.
 */
int main(int argc, char** argv)
{
    // Create the application and its window
    Game game = Game();

    game.createWindow("Hey");

    // Run the application
    game.run();

    // Clean up the application
    game.Close();
}