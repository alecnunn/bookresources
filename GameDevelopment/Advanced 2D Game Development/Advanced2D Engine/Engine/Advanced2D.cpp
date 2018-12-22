// Advanced2D Engine
// Main source code file

//includes
#include "Advanced2D.h"

namespace Advanced2D
{
	Engine::Engine()
	{
		srand((unsigned int)time(NULL));
		p_maximizeProcessor = true;
		p_frameCount_core = 0;
		p_frameRate_core = 0;
		p_frameCount_real = 0;
		p_frameRate_real = 0;
		p_ambientColor = D3DCOLOR_RGBA(255,255,255, 0);
		p_windowHandle = 0;
		p_pauseMode = false;
		p_versionMajor = VERSION_MAJOR;
		p_versionMinor = VERSION_MINOR;
		p_revision = REVISION;
		
		//set default values
		this->setAppTitle("Advanced2D");
		this->setScreenWidth(640);
		this->setScreenHeight(480);
		this->setColorDepth(32);
		this->setFullscreen(false);
		
		//window handle must be set later on for DirectX!
		this->setWindowHandle(0);
	}
	
	Engine::~Engine()
	{
		audio->StopAll();
		delete audio;
		delete p_input;
		delete math;
		if (this->p_device) this->p_device->Release();
		if (this->p_d3d) this->p_d3d->Release();
	}
	
	std::string Engine::getVersionText()
	{
		std::ostringstream s;
		s << "Advanced2D Engine v" << p_versionMajor << "." << p_versionMinor << "." << p_revision;
		return s.str();
	}
	
	void Engine::message(std::string message, std::string title)
	{
		MessageBox(0, message.c_str(), title.c_str(), 0);
	}
	
	void Engine::fatalerror(std::string message, std::string title)
	{
		this->message(message,title);
		Shutdown();
	}
	
	int Engine::Init(int width, int height, int colordepth, bool fullscreen)
	{
	   //initialize Direct3D
		this->p_d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (this->p_d3d == NULL) {
			return 0;
		}
	
		//get system desktop color depth
		D3DDISPLAYMODE dm;
		this->p_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
	
		//set configuration options for Direct3D
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = (!fullscreen);
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.BackBufferFormat = dm.Format;
		d3dpp.BackBufferCount = 1;
		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;
		d3dpp.hDeviceWindow = p_windowHandle;
	
		//create Direct3D device
		this->p_d3d->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			this->p_windowHandle,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp,
			&this->p_device);
	
		if (this->p_device == NULL) return 0;
	
		//clear the backbuffer to black
		this->ClearScene(D3DCOLOR_XRGB(0,0,0));
	
		//create pointer to the back buffer
		this->p_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &this->p_backbuffer);
	
	    //use ambient lighting and z-buffering
		this->p_device->SetRenderState(D3DRS_ZENABLE, TRUE);
		this->p_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        this->p_device->SetRenderState(D3DRS_LIGHTING, true);
		this->SetAmbient(this->p_ambientColor);
	
		//initialize 2D renderer
		HRESULT result = D3DXCreateSprite(this->p_device, &this->p_sprite_handler);
		if (result != D3D_OK) return 0;

        //initialize directinput	
		p_input = new Input(this->getWindowHandle());

		//create audio system
		audio = new Audio();
		if (!audio->Init())	return 0;

        //initialize math library
		math = new Math();

		//call game initialization extern function
		if (!game_init(this->getWindowHandle()))
			return 0;
	
		//set a default material
		SetDefaultMaterial();
	
		return 1;
	}
	
    void Engine::SetIdentity()
    {
    	D3DXMATRIX matrixWorld;
        D3DXMatrixIdentity(&matrixWorld);
        g_engine->getDevice()->SetTransform(D3DTS_WORLD, &matrixWorld);
    }
	
	void Engine::SetDefaultMaterial()
	{
	    D3DMATERIAL9 mat;
		memset(&mat, 0, sizeof(mat));
		mat.Diffuse.r = mat.Ambient.r = 1.0f;
		mat.Diffuse.g = mat.Ambient.g = 1.0f;
		mat.Diffuse.b = mat.Ambient.b = 1.0f;
		mat.Diffuse.a = mat.Ambient.a = 1.0f;
		p_device->SetMaterial(&mat);
	}
	
	
	void Engine::ClearScene(D3DCOLOR color)
	{
		this->p_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
	}
	
	void Engine::SetAmbient(D3DCOLOR colorvalue)
	{
		this->p_ambientColor = colorvalue;
		this->p_device->SetRenderState(D3DRS_AMBIENT, this->p_ambientColor);
	}
	
	
	int Engine::RenderStart()
	{
		if (!this->p_device) return 0;
		if (this->p_device->BeginScene() != D3D_OK) return 0;
		return 1;
	}
	
	int Engine::RenderStop()
	{
	   if (!this->p_device) return 0;
	   if (this->p_device->EndScene() != D3D_OK) return 0;
	   if (p_device->Present(NULL, NULL, NULL, NULL) != D3D_OK) return 0;
	   return 1;
	}
	

	int Engine::Render2D_Start()
	{
		if (p_sprite_handler->Begin(D3DXSPRITE_ALPHABLEND) != D3D_OK)
			return 0;
		else
			return 1;
	}
	
	int Engine::Render2D_Stop()
	{
	   p_sprite_handler->End();
	   return 1;
	}

	void Engine::Shutdown()
	{
		gameover = true;
	}
	
	void Engine::Update()
	{
		//calculate core framerate
		p_frameCount_core++;
		if (p_coreTimer.stopwatch(999)) {
			p_frameRate_core = p_frameCount_core;
			p_frameCount_core = 0;
		}
	
		//fast update with no timing
		game_update();

		//update entities
		if (!p_pauseMode) UpdateEntities();

		//perform global collision testing at 20 Hz
		if (!p_pauseMode && collisionTimer.stopwatch(50)) 
		{
			TestForCollisions();
		}

		//update with 60fps timing
		if (!timedUpdate.stopwatch(14)) {
			if (!this->getMaximizeProcessor()) 
			{
				Sleep(1);
			}
		}
		else {
			//calculate real framerate
			p_frameCount_real++; 
			if (p_realTimer.stopwatch(999)) {
				p_frameRate_real = p_frameCount_real;
				p_frameCount_real = 0;
			}
	
			//update input devices
			p_input->Update();
			this->UpdateKeyboard();
			this->UpdateMouse();

			//update audio system
			audio->Update();

			//begin rendering
			this->RenderStart();
 
            //call game 3d rendering
            game_render3d();

			//render 3D entities 
			if (!p_pauseMode) Draw3DEntities();

            //begin 2d rendering
			Render2D_Start();

			//render 2D entities 
			if (!p_pauseMode) Draw2DEntities();

            //let game do 2d rendering
			game_render2d();

            //done with 2d rendering
			Render2D_Stop();
        			
			//done rendering
			this->RenderStop();
		}

		//remove dead entities from the list	
		BuryEntities();
	}
	
	void Engine::UpdateMouse()
	{
		static int oldPosX = 0;
		static int oldPosY = 0;
		int deltax = p_input->GetDeltaX();
		int deltay = p_input->GetDeltaY();
	
		//check mouse buttons
		for (int n=0; n<4; n++) 
		{
			if (p_input->GetMouseButton(n))
				game_mouseButton(n);
		}
	
		//check mouse position
		if (p_input->GetPosX() != oldPosX || p_input->GetPosY() != oldPosY) {
			game_mouseMove(p_input->GetPosX(), p_input->GetPosY() );
			oldPosX = p_input->GetPosX();
			oldPosY = p_input->GetPosY();
		}
	
		//check mouse motion
		if (deltax != 0 || deltay ) {
			game_mouseMotion(deltax,deltay);
		}
	
		//check mouse wheel
		int wheel = p_input->GetDeltaWheel();
		if (wheel != 0) 
			game_mouseWheel(wheel);
	
	}
	
	void Engine::UpdateKeyboard()
	{
		static char old_keys[256];
	
		for (int n=0; n<255; n++) {
			//check for key press
			if (p_input->GetKeyState(n) & 0x80) {
				game_keyPress(n);
				old_keys[n] = p_input->GetKeyState(n);
			} 
			//check for release
			else if (old_keys[n] & 0x80) {
				game_keyRelease(n);
				old_keys[n] = p_input->GetKeyState(n);
			}
		}
	}
	
		void Engine::Close()
		{
			try {
				gameover = true;
			}
			catch (...) { }
		}
		

		void Engine::UpdateEntities()
		{
			std::list<Entity*>::iterator iter;
			Entity *entity;
		
			iter = p_entities.begin();
			while (iter != p_entities.end())
			{
				//point local sprite to object in the list
				entity = *iter;
				
				//is this entity alive?
				if ( entity->getAlive() ) {

					//move/animate entity
					entity->move();
					entity->animate();

					//tell game that this entity has been updated
					game_entityUpdate( entity );

					//see if this entity will auto-expire
					if ( entity->getLifetime() > 0) 
					{
						if ( entity->lifetimeExpired() ) 
						{
							entity->setAlive(false);
						}
					}
				}
				++iter;
			} 
		}
		
		void Engine::Draw3DEntities()
		{
			std::list<Entity*>::iterator iter;
			Entity *entity;
			
			iter = p_entities.begin();
			while (iter != p_entities.end())
			{
				//temporary pointer
				entity = *iter;
				
				//is this a 3D entity?
				if ( entity->getRenderType() == RENDER3D ) {

					//is this entity in use?
					if ( entity->getAlive() && entity->getVisible() ) {
						entity->draw();
						game_entityRender( entity );
					}
				}

				++iter;
			} 
		}

        /*  Modified from original.
            Now, when invisible, game_entityRender is still called.
        */
		void Engine::Draw2DEntities()
		{
			Entity *entity;
			std::list<Entity*>::iterator iter = p_entities.begin();
			while (iter != p_entities.end())
			{
				//temporary pointer
				entity = *iter;
				
				//is this a 2D entity?
				if ( entity->getRenderType() == RENDER2D ) {

					//is this entity in use?
					if ( entity->getAlive() ) {
    
                        if ( entity->getVisible() ) 
    						entity->draw();

                        game_entityRender( entity );
					}
				}

				++iter;
			} 
		}

		void Engine::BuryEntities()
		{
			std::list<Entity*>::iterator iter = p_entities.begin();
			while (iter != p_entities.end())
			{
				if ( (*iter)->getAlive() == false )  
				{
					delete (*iter);
					iter = p_entities.erase( iter );
				}
				else iter++;
			}
		}
			
		Entity *Engine::findEntity(std::string name)
		{
			std::list<Entity*>::iterator i = p_entities.begin();
			while (i != p_entities.end())
			{
				if ( (*i)->getAlive() == true && (*i)->getName() == name ) 
					return *i;
				else
					++i;
			}
			
			return NULL;
		}
		
		Entity *Engine::findEntity(int objectType)
		{
			std::list<Entity*>::iterator i = p_entities.begin();
			while (i != p_entities.end())
			{
				if ( (*i)->getAlive() == true && (*i)->getObjectType() == objectType ) 
					return *i;
				else
					++i;
			}
			return NULL;
		}

		int Engine::getEntityCount(int objectType)
		{
			int total = 0;
			std::list<Entity*>::iterator i = p_entities.begin();
			while (i != p_entities.end())
			{
				if ( (*i)->getAlive() == true && (*i)->getObjectType() == objectType ) 
					total++;
				else
					++i;
			}
			return total;
		}

		void Engine::addEntity(Entity *entity)
		{
			static int id = 0;
			
			entity->setID(id);
			p_entities.push_back(entity);
			id++;
		}

		bool Engine::collision(Sprite *sprite1, Sprite *sprite2)
		{
			switch (sprite1->getCollisionMethod()) {
				case COLLISION_RECT:
					return collisionBR(sprite1,sprite2);
					break;
				case COLLISION_DIST:
					return collisionD(sprite1,sprite2);
					break;
				case COLLISION_NONE:
				default:
					return false;
			}
		}

        //Modified from original
		bool Engine::collisionBR(Sprite *sprite1, Sprite *sprite2) 
		{
			bool ret = false;

			Rect *ra = new Rect(
				sprite1->getX(),
				sprite1->getY(),
				sprite1->getX() + sprite1->getWidth() * sprite1->getScale(), 
				sprite1->getY() + sprite1->getHeight() * sprite1->getScale() );
			Rect *rb = new Rect(
				sprite2->getX(), 
				sprite2->getY(),
				sprite2->getX() + sprite2->getWidth() * sprite2->getScale(), 
				sprite2->getY() + sprite2->getHeight() * sprite2->getScale() );

			//are any of sprite b's corners intersecting sprite a?
			if (ra->isInside( rb->getLeft(), rb->getTop() ) ||
				ra->isInside( rb->getRight(), rb->getTop() ) ||
				ra->isInside( rb->getLeft(), rb->getBottom() ) ||
				ra->isInside( rb->getRight(), rb->getBottom() )) 
                    ret = true;

/*
            Rect ra = sprite1->getBounds();
            Rect rb = sprite2->getBounds();

            Vector3 upperLeft( rb.left, rb.top, 0.0 );
            Vector3 lowerLeft( rb.left, rb.bottom, 0.0 );
            Vector3 upperRight( rb.right, rb.top, 0.0 );
            Vector3 lowerRight( rb.right, rb.bottom, 0.0 );

        	if (ra.isInside( upperLeft ) || ra.isInside( lowerLeft ) ||
				ra.isInside( upperRight ) || ra.isInside( lowerRight ))
                    ret = true;
*/
            delete ra;
            delete rb;
			return ret;
		}

	bool Engine::collisionD(Sprite *sprite1, Sprite *sprite2)
		{
			double radius1, radius2;

			//calculate radius 1
			if (sprite1->getWidth() > sprite1->getHeight())
				radius1 = (sprite1->getWidth()*sprite1->getScale())/2;
			else
				radius1 = (sprite1->getHeight()*sprite1->getScale())/2;

			//point = center of sprite 1
			double x1 = sprite1->getX() + radius1;
			double y1 = sprite1->getY() + radius1;
			Vector3 vector1(x1, y1, 0.0);

			//calculate radius 2
			if (sprite2->getWidth() > sprite2->getHeight())
				radius2 = (sprite2->getWidth()*sprite2->getScale())/2;
			else
				radius2 = (sprite2->getHeight()*sprite2->getScale())/2;

			//point = center of sprite 2
			double x2 = sprite2->getX() + radius2;
			double y2 = sprite2->getY() + radius2;
			Vector3 vector2(x2, y2, 0.0);

			//calculate distance
			double dist = vector1.Distance( vector2 );

			//return distance comparison
			return (dist < radius1 + radius2);
		}

        /*
            Modified from original.
            Now invisible sprites may still collide.
        */
		void Engine::TestForCollisions()
		{
			std::list<Entity*>::iterator first;
			std::list<Entity*>::iterator second;
			Sprite *sprite1;
			Sprite *sprite2;
		
			first = p_entities.begin();
			while (first != p_entities.end() )
			{
				//we only care about sprite collisions
				if ( (*first)->getRenderType() == RENDER2D )
				{
					//point local sprite to sprite contained in the list
					sprite1 = (Sprite*) *first;
					
					//if this entity is alive and visible...
					//if ( sprite1->getAlive() && sprite1->getVisible() && sprite1->isCollidable() )
                    if ( sprite1->getAlive() && sprite1->isCollidable() )
					{
						//test all other entities for collision
						second = p_entities.begin();
						while (second != p_entities.end() )
						{
							//point local sprite to sprite contained in the list
							sprite2 = (Sprite*) *second;
							
							//if other entity is active and not same as first entity...
							//if ( sprite2->getAlive() && sprite2->getVisible() && sprite2->isCollidable() && sprite1 != sprite2 ) 
                            if ( sprite2->getAlive() && sprite2->isCollidable() && sprite1 != sprite2 ) 
							{
								//test for collision
								if ( collision(sprite1, sprite2 ) ) {
								
									//notify game of collision
									game_entityCollision( sprite1, sprite2 );
								}
	
							}
							//go to the next sprite in the list
							second++;
						}
			
					} 
					//go to the next sprite in the list
					first++;
				}//render2d
			} //while
		} 


} //namespace
