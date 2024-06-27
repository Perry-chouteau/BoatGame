
#ifndef CANON_HPP
#define CANON_HPP
#include "ogl_headers.h"
#include <cyclone.h>
#include "timing.h"

enum BulletType {
  UNUSED,
  DIRECT,
  DIPPING,
};

class Bullet : public cyclone::CollisionSphere {
  public:
    BulletType type;
    cyclone::Particle particle;
    unsigned int startTime;

    Bullet(): type(UNUSED) {
      body = new cyclone::RigidBody();
    }

    ~Bullet() {
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
      glutSolidSphere(radius, 20, 20);
      glPopMatrix();
    }

    void fire(BulletType bulletType, cyclone::Quaternion orientation, cyclone::Vector3 position) {
      // Implement the firing logic here
      type = bulletType;
 
      switch (type) {
        case DIRECT:
          body->setMass(200.f);
          body->setVelocity((orientation * cyclone::Vector3(0.0f, 0.f, -25.0f)));

          body->setAcceleration(0.0f, -5.f, 0.0f);
          body->setDamping(0.99f, 0.8f);
          radius = 0.2f;
          break;
        case DIPPING:
          body->setMass(200.f);
          body->setVelocity((orientation * cyclone::Vector3(0.0f, 15.f, -20.0f)));
          body->setAcceleration(0.0f, -20.f, 0.0f);
          body->setDamping(0.99f, 0.8f);
          radius = 0.2f;
          break;
      }

      body->setCanSleep(false);
        body->setAwake();

        cyclone::Matrix3 tensor;
        cyclone::real coeff = 0.4f*body->getMass()*radius*radius;
        tensor.setInertiaTensorCoeffs(coeff,coeff,coeff);
        body->setInertiaTensor(tensor);

        // Set the data common to all particle types
        body->setPosition(position.x, 1.f, position.z);
        startTime = TimingData::get().lastFrameTimestamp;

        // Clear the force accumulators
        body->calculateDerivedData();
        calculateInternals();
    }
};

class Canon {

  public:
    const static unsigned magSize = 4;

    Bullet mag[magSize];

    BulletType currentBulletType;
    cyclone::Quaternion orientation;
    cyclone::Vector3 position;

    Canon(): currentBulletType (BulletType::DIRECT) {
    }

    void setOrientation(cyclone::Quaternion orientation, cyclone::Vector3 position) {
      this->orientation = orientation;
      this->position = position;
    }

    void update(cyclone::real duration) {
      for (Bullet *bullet = mag; bullet < mag+magSize; bullet++) {
        if (bullet->type != UNUSED) {
            // Run the physics
            bullet->body->integrate(duration);
            bullet->calculateInternals();

            // Check if the particle is now invalid
            if (bullet->body->getPosition().y < -5.0f ||
                bullet->startTime+5000 < TimingData::get().lastFrameTimestamp ||
                bullet->body->getPosition().z > this->position.z + 200.0f)
            {
                // We simply set the shot type to be unused, so the
                // memory it occupies can be reused by another shot.
                bullet->type = UNUSED;
            }
        }
      }
    }

    void setBulletType(BulletType type) {
      currentBulletType = type;
    }

    void fire() {
      Bullet *bullet;

      for (bullet = mag; bullet < mag+magSize; bullet++) {
        if (bullet->type == UNUSED) break;
      }
      
      // If we didn't find a round, then exit - we can't fire.
      if (bullet >= mag+magSize) return;

      // shoot
      bullet->fire(currentBulletType, orientation, position);
    }

    void render() {
      for (Bullet *bullet = mag; bullet < mag+magSize; bullet++) {
        if (bullet->type != UNUSED) {
          bullet->render();
        }
      }
    }

};

#endif // CANON_HPP