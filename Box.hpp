

#include <cyclone.h>
#include "ogl_headers.h"
#include "timing.h"

class Box : public cyclone::CollisionBox//, public cyclone::ParticleBuoyancy
{
public:
    bool used;

    Box()
    {
        used = true;

        body = new cyclone::RigidBody;

        cyclone::real x = (rand() % 24) - 12; 
        cyclone::real y = (rand() % 24) - 12;

/*        while (x < 10 && x > -10) {
          x = (rand() % 24) - 12;
          y = (rand() % 24) - 12;
        }*/

        body->setPosition(x, 1, y);
        body->setOrientation(1,0,0,0);
        body->setVelocity(0,0,0);
        body->setRotation(0,0,0);

        halfSize = cyclone::Vector3(1,1,1);
//        cyclone::real mass = halfSize.x * halfSize.y * halfSize.z * 8.0f;
        body->setMass(400);

        cyclone::Matrix3 tensor;
        tensor.setBlockInertiaTensor(cyclone::Vector3(2,1,1), 100.0f);
        body->setInertiaTensor(tensor);
        

        body->setLinearDamping(0.95f);
        body->setAngularDamping(0.8f);
        body->clearAccumulators();
        body->setAcceleration(0,-10.0f,0);

        body->setCanSleep(false);
        body->setAwake();

        body->calculateDerivedData();
        calculateInternals();
        //this->buoyancy = buoyancy;
    }

    ~Box()
    {
        delete body;
    }

    /** Draws the box, excluding its shadow. */
    void render()
    {
        // Get the OpenGL transformation
        GLfloat mat[16];
        body->getGLTransform(mat);

        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(halfSize.x*2, halfSize.y*2, halfSize.z*2);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
};
//
class TargetGenerator
{
public:
    const static unsigned storageSize = 10;
    Box boxes[storageSize];

    TargetGenerator() {
      Box *box;
      for (box = boxes; box < boxes+storageSize; box++)
      {
        box->used = false;
      }
    }

    void respawn(cyclone::Vector3 player, cyclone::ForceRegistry& registry, cyclone::Buoyancy& buoyancy) {
      Box *box;
      int i = 0;
      for (box = boxes; box < boxes+storageSize; box++) {
        if (box->used == false) {
          box->used = true;
          cyclone::real newX = player.x + (rand() % 10 - 5); 
          cyclone::real newZ = player.z + (rand() % 10 - 5);
          box->body->setPosition(newX, player.y ,newZ);
          registry.add(box->body, &buoyancy);
        }; 
      }
    }

    void hit(Box *box, cyclone::ForceRegistry& registry, cyclone::Buoyancy& buoyancy) {
      registry.remove(box->body, &buoyancy);
    }

    void update(cyclone::Vector3 player, cyclone::ForceRegistry& registry, cyclone::Buoyancy& buoyancy, cyclone::real duration) {

      Box*box;
      for (box = boxes; box < boxes+ storageSize; box++)
      {
        if (box->used) {
          if (box->body->getPosition().y < -5.0f) {
            box->used = false;

            respawn(player, registry, buoyancy); 
          }
        }
      }
    }

    void render()
    {
      Box*box;
      for (box = boxes; box < boxes+ storageSize; box++) {
          if (box->used) {
            box->render();
          }
        }
    }

};