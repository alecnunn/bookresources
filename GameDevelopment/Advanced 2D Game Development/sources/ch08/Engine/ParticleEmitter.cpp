/*
    Added lifetime property after book went to print.
*/

#include "Advanced2D.h"

namespace Advanced2D {

    ParticleEmitter::ParticleEmitter()
    {
        //initialize particles to defaults
        image = NULL;
        max = 100;
        length = 100;
        direction = 0;
        alphaMin = 254; alphaMax = 255;
        minR = 0; maxR = 255;
        minG = 0; maxG = 255;
        minB = 0; maxB = 255;
        spread = 10;
        velocity = 1.0f;
        scale = 1.0f;
        lifetime = 0;
    }
    
    bool ParticleEmitter::loadImage(std::string imageFile)
    {
        image = new Texture();
        return image->Load(imageFile);
    }
    
    
    ParticleEmitter::~ParticleEmitter()
    {
        delete image;
    
        //destroy particles
        for (iter i = particles.begin(); i != particles.end(); ++i)
        {
            delete *i;
        }
        particles.clear();
    }
    
    void ParticleEmitter::add()
    {
        static double PI_DIV_180 = 3.1415926535 / 180.0f;
        double vx,vy;
        Sprite *p;

        p = new Sprite();
        p->setImage(image);
        p->setPosition(position.getX(), position.getY());

        //add some randomness to the spread
        double variation = (rand() % spread - spread/2) / 100.0f;

        //set linear velocity
        double dir = direction - 90.0;
        vx = cos( dir * PI_DIV_180) + variation;
        vy = sin( dir * PI_DIV_180) + variation;
        p->setVelocity(vx * velocity,vy * velocity);

        //set random color based on ranges
        int r = rand()%(maxR-minR)+minR;
        int g = rand()%(maxG-minG)+minG;
        int b = rand()%(maxB-minB)+minB;
        int a = rand()%(alphaMax-alphaMin)+alphaMin;
        p->setColor(D3DCOLOR_RGBA(r,g,b,a));

        //lifetime
        p->setLifetime(lifetime);
    
        //set the scale
        p->setScale( scale );
    
        //add particle to the emitter
        particles.push_back(p);
    
    }
    
    void ParticleEmitter::draw()
    {
        //draw particles
        for (iter i = particles.begin(); i != particles.end(); ++i)
        {
            (*i)->draw();
        }
    }
    
    /*
        This newly modified update() now includes an optional delay value
        to slow down the adding of new particles (up to the limit).
    */
    void ParticleEmitter::update(int ms)
    {
        static Timer timer;
        
        //do we need to add a new particle?
        if ((int)particles.size() < max) 
        {
            //trivial but necessary slowdown
            if (timer.stopwatch(ms)) add();
        }
    
        
        for (iter i = particles.begin(); i != particles.end(); ++i)
        {
            //has lifetime expired?
            if ((*i)->getLifetime() > 0) {
                if ((*i)->lifetimeExpired())
					i = particles.erase(i);
            }

            //update particle's position
            (*i)->move();
            
            //is particle beyond the emitter's range?
            if ( (*i)->getPosition().Distance(this->position) > length)
            {
                //reset particle to the origin
                (*i)->setX(position.getX());
                (*i)->setY(position.getY());
            }
        }
    }

    void ParticleEmitter::setAlphaRange(int min,int max) 
    { 
        alphaMin=min; 
        alphaMax=max; 
    }

    void ParticleEmitter::setColorRange(int r1,int g1,int b1,int r2,int g2,int b2)
    {
        minR = r1; maxR = r2;
        minG = g1; maxG = g2;
        minB = b1; maxB = b2;
    }

    void ParticleEmitter::setLifetime(int ms)
    {
        lifetime = ms;
    }

}


