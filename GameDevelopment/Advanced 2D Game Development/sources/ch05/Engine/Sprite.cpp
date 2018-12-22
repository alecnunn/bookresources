#include "Advanced2D.h"

namespace Advanced2D {

    Sprite::Sprite()
    {
        this->image = NULL;
        this->imageLoaded = false;
        this->setPosition(0.0f,0.0f);
        this->setVelocity(0.0f,0.0f);
        this->state = 0;
        this->direction = 0;
        this->width = 1;
        this->height = 1;
        this->curframe = 0;
        this->totalframes = 1;
        this->animdir = 1;
        this->animcolumns = 1;
        this->framestart = 0;
        this->frametimer = 0;
        this->animcolumns = 1;
        this->animstartx = 0;
        this->animstarty = 0;
        this->faceAngle = 0;
        this->moveAngle = 0;
        this->rotation = 0;
        this->scaling = 1.0f;
        this->color = 0xFFFFFFFF;
        this->movetimer = 16;
        this->movestart = 0;
        this->collidable = true;
        this->collisionMethod = COLLISION_RECT;
    }
    
    Sprite::~Sprite()  {
        if (imageLoaded)
            delete image;
    }
    
    bool Sprite::loadImage(std::string filename, D3DCOLOR transcolor)
    {
        //if image already exists, free it
        if (imageLoaded && image != NULL) delete image;
    
        //create texture and load image
        image = new Texture();
        if (image->Load(filename,transcolor))
        {
            this->setSize(image->getWidth(),image->getHeight());
            imageLoaded = true;
            return true;
        }
        else
            return false;
    }
    
    void Sprite::setImage(Texture *image)
    {
        this->image = image;
        this->setWidth(image->getWidth());
        this->setHeight(image->getHeight());
        this->imageLoaded = false;
    }
    
    void Sprite::transform()
    {
        D3DXMATRIX mat;
        D3DXVECTOR2 scale((float)scaling,(float)scaling);
        D3DXVECTOR2 center((float)(width*scaling)/2, (float)(height*scaling)/2);
        D3DXVECTOR2 trans((float)getX(), (float)getY());
        D3DXMatrixTransformation2D(&mat,NULL,0,&scale,&center,(float)rotation,&trans);
        g_engine->getSpriteHandler()->SetTransform(&mat);
    }

    void Sprite::draw()
    {
        //calculate source frame location
        int fx = (this->curframe % this->animcolumns) * this->width;
        int fy = (this->curframe / this->animcolumns) * this->height;
        RECT srcRect = {fx,fy, fx+this->width, fy+this->height};

        //draw the sprite frame
        this->transform();
        g_engine->getSpriteHandler()->Draw(this->image->GetTexture(),&srcRect,NULL,NULL,color);
    }
    
    void Sprite::move()
    {
        if (movetimer > 0) {

            if (timeGetTime() > (DWORD)(movestart + movetimer)) {
                //reset move timer
                movestart = timeGetTime();

                //move sprite by velocity amount
                this->setX(this->getX() + this->velocity.getX());
                this->setY(this->getY() + this->velocity.getY());
            }
        }
        else {
            //no movement timer--update at cpu clock speed
            this->setX(this->getX() + this->velocity.getX());
            this->setY(this->getY() + this->velocity.getY());
        }
    }
    
    void Sprite::animate() 
    {
        //update frame based on animdir
        if (frametimer > 0) {

            if (timeGetTime() > (DWORD)(framestart + frametimer))  {
                //reset animation timer
                framestart = timeGetTime();
                curframe += animdir;

                //keep frame within bounds    
                if (curframe < 0) curframe = totalframes-1;
                if (curframe > totalframes-1) curframe = 0;
            }
        }
        else {
            //no animation timer--update at cpu clock speed
            curframe += animdir;
            if (curframe < 0) curframe = totalframes-1;
            if (curframe > totalframes-1) curframe = 0;
        }
    }

}
