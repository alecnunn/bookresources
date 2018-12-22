// DirectX Display

#pragma once

#define VC_EXTRALEAN 
#define WIN32_LEAN_AND_MEAN 

#include <windows.h>
#include <windowsx.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "Light.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Utility.h"
#include "Timer.h"
#include "LineEmitter.h"
#include "QuadEmitter.h"
#include "System/Key.h"
#include "System/Mouse.h"
#include "System/DisplayAdapter.h"


namespace DirectX
{
	typedef System::DisplayAdapter SystemDisplay;

    /// %DirectX implementation of %Display

    class Display : public System::DisplayAdapter
	{
		LPDIRECT3D9 _d3d;                       ///< d3d object

		LPDIRECT3DDEVICE9 _device;              ///< d3d device
		LPD3DXFONT _font;                       ///< d3dx font to be used for fps rendering

		HWND _window;                           ///< display window handle

        typedef std::vector<Light*> Lights;     ///< convenient array of lights type

		Texture *_texture;                      ///< current selected texture
		Material *_material;                    ///< current selected material
        Lights _lights;                         ///< array of current selected lights
		Mesh *_mesh;                            ///< current selected mesh

		int _translate[256];                    ///< translation mapping for key codes

        bool _active;                           ///< true if the window is currently active (in foreground)

        bool _stats;                            ///< controls if rendering stats are displayed
        Timer _timer;                           ///< timer used for fps stats
        double _fps;                            ///< most recent smoothed fps

        bool _wireframe;                        ///< controls if wireframe render mode is engaged

        LineEmitter _lineEmitter;               ///< line emitter
        QuadEmitter _quadEmitter;               ///< quad emitter

	protected:

        /// set members data to defaults

		void defaults()
		{
			_device = 0;
			_font = 0;
			_window = 0;

			_texture = 0;
			_material = 0;
			_mesh = 0;

            _lights.clear();
            for (int i=0; i<8; i++) _lights.push_back(0);

			_stats = true;
            _fps = 60;

            _wireframe = false;

            _active = false;
		}

        /// static window procedure mapping to display instance WindowProc via window class user data.

		static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			LONG_PTR extra = GetWindowLongPtr(hWnd, GWL_USERDATA);

			if (!extra) return DefWindowProc(hWnd,uMsg,wParam,lParam);

			Display *display = (Display*) extra;

			return display->WindowProc(hWnd, uMsg, wParam, lParam);
		}

        /// the actual display instance window procedure

		LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg)
			{
				case WM_PAINT:
				{
					ValidateRect(hWnd, NULL);
				}
				break;

				case WM_KEYDOWN:
				{
					notifyKeyDown(_translate[wParam&0xFF]);

                    switch (wParam)
                    {
                        case VK_DELETE:
                            _stats = !_stats;
                            break;

                        case VK_INSERT:
                            _wireframe = !_wireframe;
                            if (_wireframe)
                                _device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
                            else
                                _device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
                            break;
                    }
				}
				break;

				case WM_KEYUP:
				{
					notifyKeyUp(_translate[wParam&0xFF]);
				}
				break;

				case WM_LBUTTONDOWN:
				{
					const int x = GET_X_LPARAM(lParam); 
					const int y = GET_Y_LPARAM(lParam);
					notifyMouseDown(System::Mouse(System::Mouse::LEFT),x,y);

                    SetCapture(_window);
                }
				break;

				case WM_MBUTTONDOWN:
				{
					const int x = GET_X_LPARAM(lParam); 
					const int y = GET_Y_LPARAM(lParam);
					notifyMouseDown(System::Mouse(System::Mouse::MIDDLE),x,y);

                    SetCapture(_window);
                }
				break;

				case WM_RBUTTONDOWN:
				{
					const int x = GET_X_LPARAM(lParam); 
					const int y = GET_Y_LPARAM(lParam);
					notifyMouseDown(System::Mouse(System::Mouse::RIGHT),x,y);

                    SetCapture(_window);
				}
				break;

				case WM_LBUTTONUP:
				{
					const int x = GET_X_LPARAM(lParam); 
					const int y = GET_Y_LPARAM(lParam);
					notifyMouseUp(System::Mouse(System::Mouse::LEFT),x,y);

                    ReleaseCapture();
                }
				break;

				case WM_MBUTTONUP:
				{
					const int x = GET_X_LPARAM(lParam); 
					const int y = GET_Y_LPARAM(lParam);
					notifyMouseUp(System::Mouse(System::Mouse::MIDDLE),x,y);

                    ReleaseCapture();
				}
				break;

				case WM_RBUTTONUP:
				{
					const int x = GET_X_LPARAM(lParam); 
					const int y = GET_Y_LPARAM(lParam);
					notifyMouseUp(System::Mouse(System::Mouse::RIGHT),x,y);

                    ReleaseCapture();
				}
				break;

				case WM_MOUSEMOVE:
				{
					const int x = GET_X_LPARAM(lParam); 
					const int y = GET_Y_LPARAM(lParam);
					notifyMouseMove(System::Mouse(System::Mouse()),x,y);
				}
				break;

				case WM_ACTIVATE:
				{
                    _active = wParam != WA_INACTIVE;
					notifyActivate(_active);
				}
				break;

				case WM_CLOSE:
				{
                    if (notifyClose())
                    {
					    close();
					    exit(0);
                    }
				}
				break;

				default:
		            return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}

			return 0;
		}

	public:

        /// default constructor.
        /// clears member data to defaults and creates d3d object.

		Display()
		{
			defaults();

			_d3d = Direct3DCreate9(D3D_SDK_VERSION);
		}

        /// destructor.
        /// closes display and releases d3d object.

		virtual ~Display()
		{
			close();

			_d3d->Release();
			_d3d = 0;
		}

		virtual void open(const char title[], int width, int height, bool windowed)
		{
			close();

            SystemDisplay::open(title, width, height, windowed);

			// register window class

			LOGBRUSH description;
			description.lbStyle = BS_SOLID;
			description.lbColor = DIB_RGB_COLORS;
			description.lbHatch = 0;
			HBRUSH black = CreateBrushIndirect(&description);
    
			WNDCLASSEX winclass;
			winclass.cbSize = sizeof(WNDCLASSEX);
			winclass.style = CS_DBLCLKS;
			winclass.lpfnWndProc = &StaticWindowProc;
			winclass.cbClsExtra = 0;
			winclass.cbWndExtra = 0;
			HINSTANCE instance = GetModuleHandle(0);
			winclass.hInstance = instance;
			winclass.hIcon = LoadIcon(instance,"SYSTEM_ICON");
			winclass.hCursor = LoadCursor(NULL,IDC_ARROW);
			winclass.hbrBackground = black;
			winclass.lpszMenuName = NULL;
			winclass.lpszClassName = title;
			winclass.hIconSm = NULL;

			UnregisterClass(title, instance);
			if (!RegisterClassEx(&winclass)) throw Error("failed to register window class");

			// create window

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = width;
			rect.bottom = height;
			AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,0);
			rect.right -= rect.left;
			rect.bottom -= rect.top;

			int x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) >> 1;
			int y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) >> 1;

			HWND hWnd = CreateWindow(title,title,WS_OVERLAPPEDWINDOW,x,y,rect.right,rect.bottom,NULL,NULL,instance,NULL);
			if (!hWnd) throw Error("failed to create window");

			SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)(LONG_PTR) this);

			ShowWindow(hWnd,SW_NORMAL);
			
			_window = hWnd;

			// create device

			if (!_d3d) 
                throw Error("invalid d3d device");

			D3DPRESENT_PARAMETERS d3dpp; 
			ZeroMemory(&d3dpp, sizeof(d3dpp)); 
    
			if (!windowed)
			{
				d3dpp.Windowed = FALSE;
				d3dpp.BackBufferWidth = width;
				d3dpp.BackBufferHeight = height;
				d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
				d3dpp.BackBufferCount = 3;
				d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
				d3dpp.EnableAutoDepthStencil = TRUE;
				d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
                d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
			}
			else
			{
				D3DDISPLAYMODE mode;
				if (FAILED(_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode))) 
                    throw Error("get adapter display mode failed");
				d3dpp.Windowed = TRUE;
				d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
				d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
				d3dpp.BackBufferCount = 1;
				d3dpp.EnableAutoDepthStencil = TRUE;
				d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
                d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
			}

			if( FAILED( _d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &_device ) ) )
			{
				if( FAILED( _d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &_device ) ) )
				{
					HRESULT result = _d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &_device );

                    if (result!=D3D_OK)
					{
						switch (result)
						{
							case D3DERR_INVALIDCALL: throw Error("failed to create device - invalid call");
							case D3DERR_NOTAVAILABLE: throw Error("failed to create device - not available");
							case D3DERR_OUTOFVIDEOMEMORY: throw Error("failed to create device - out of video memory");
						}

                        throw Error("failed to create device");
					}
				}
			}

            // create font

			assert(_device);

			D3DXCreateFont(_device,(HFONT)GetStockObject(SYSTEM_FONT),&_font);
			if (!_font) throw Error("failed to create font");

			// initialize states

			if (_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR) != D3D_OK) throw Error("set texture magfilter failed");
			if (_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR) != D3D_OK) throw Error("set texture minfilter failed");
			if (_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR) != D3D_OK) throw Error("set texture mipfilter failed");

			if (_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW) != D3D_OK) 
                throw Error("set cull mode failed");

			if (_device->SetRenderState(D3DRS_ZENABLE, TRUE) != D3D_OK) throw Error("depth buffer enable failed");
			if (_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE) != D3D_OK) throw Error("depth buffer write enable failed");
    
			if (_device->SetFVF(D3DFVF_VERTEX) != D3D_OK) 
                throw Error("set vertex shader failed");

			if (_device->SetRenderState(D3DRS_LIGHTING, TRUE) != D3D_OK) 
                throw Error("enable lighting failed");

            if (_device->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF) != D3D_OK) 
                throw Error("enable ambient lighting failed");

            if (_device->SetRenderState(D3DRS_SPECULARENABLE, TRUE) != D3D_OK) 
                throw Error("enable specular lighting failed");
	
            _device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

			// clear back buffers

			clear(Color(0,0,0), 1.0f, 0);
            update();
			clear(Color(0,0,0), 1.0f, 0);
            update();
			clear(Color(0,0,0), 1.0f, 0);
            update();

			// setup win32 key translation table

			for (int i=0; i<256; i++) _translate[i] = i;

			_translate[219] = System::Key::OPENBRACKET;
			_translate[221] = System::Key::CLOSEBRACKET;
			_translate[220] = System::Key::BACKSLASH;
			_translate[13]  = System::Key::ENTER;
			_translate[187] = System::Key::EQUALS;
			_translate[189] = System::Key::SEPARATOR;
			_translate[186] = System::Key::SEMICOLON;
			_translate[191] = System::Key::SLASH;
			_translate[190] = System::Key::PERIOD;
			_translate[188] = System::Key::COMMA;
			_translate[45]  = System::Key::INSERT;
			#undef DELETE
			_translate[46]  = System::Key::DELETE;

            // initialize emitters

            _lineEmitter.initialize(_device, 1024);
            _quadEmitter.initialize(_device, 1024);
        }

		virtual void close()
		{
            _lineEmitter.release();
            _quadEmitter.release();

			if (_window) DestroyWindow(_window);

			if (_font) _font->Release();
			if (_device) _device->Release();
			
			defaults();
		}

		virtual bool valid() const
		{
			return _d3d && _device && _window && _font;
		}

		virtual void begin()
		{
			assert(_device);

			if (_device->BeginScene()!=D3D_OK) 
                throw Error("begin scene failed");
		}

		virtual void end()
		{
			assert(_font);

			assert(_device);

            _timer.time();
            double delta = _timer.delta();

            double fps = 1.0 / delta;

            _fps += (fps - _fps) * 0.05;

			if (_stats)
			{
				char str[64];
				sprintf(str,"%.2f fps", _fps);

                _font->Begin();

				RECT r;	r.top=10;r.left=10;r.bottom=40;r.right=100;
				_font->DrawText(str,-1,&r,DT_LEFT|DT_TOP,0xFFFFFFFF);

                _font->End();
			}

			if (_device->EndScene()!=D3D_OK) 
                throw Error("end scene failed");
		}

		virtual void clear(const Color &color, float depth, unsigned int stencil)
		{
			assert(_device);

			if (_device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL , color.packed(), depth, stencil) != D3D_OK) throw Error("clear failed");
		}               

		virtual void update()
		{
			assert(_device);

			SystemDisplay::update();

			MSG message;

            while (true)
            {
                if (_active)
                {
			        if (!PeekMessage(&message,_window,0,0,PM_REMOVE)) break;
				    TranslateMessage(&message);
				    DispatchMessage(&message);
			    }
                else
                {
			        if (!GetMessage(&message,_window,0,0)) break;
				    TranslateMessage(&message);
				    DispatchMessage(&message);
                }
            }

			if (_device->Present( NULL, NULL, NULL, NULL ) != D3D_OK) throw Error("present failed");

            Sleep(0);
		}

		virtual int numberOfLights() const
		{
			return (int) _lights.size();
		}

		virtual int numberOfTextureStages() const
		{
            // note: we currently only support a single texture stage

			return 1;
		}

		virtual void world(const Matrix &matrix)
		{
            D3DXMATRIX d3d_matrix = ConvertMatrix(matrix);
			if (_device->SetTransform(D3DTS_WORLD, &d3d_matrix) != D3D_OK) throw Error("set world transform failed");
		}

		virtual void view(const Matrix &matrix)
		{
            D3DXMATRIX d3d_matrix = ConvertMatrix(matrix);
			if (_device->SetTransform(D3DTS_VIEW, &d3d_matrix) != D3D_OK) throw Error("set view transform failed");
		}

		virtual void projection(const Matrix &matrix)
		{
            D3DXMATRIX d3d_matrix = ConvertMatrix(matrix);
			if (_device->SetTransform(D3DTS_PROJECTION, &d3d_matrix) != D3D_OK) throw Error("set projection transform failed");
		}
		
		virtual Matrix world() const
		{
			D3DXMATRIX matrix;
			if (_device->GetTransform(D3DTS_WORLD, &matrix) != D3D_OK) throw Error("get world transform failed");
			return ConvertMatrix(matrix);
		}

		virtual Matrix view() const
		{
			D3DXMATRIX matrix;
			if (_device->GetTransform(D3DTS_VIEW, &matrix) != D3D_OK) throw Error("get view transform failed");
			return ConvertMatrix(matrix);
		}

		virtual Matrix projection() const
		{
			D3DXMATRIX matrix;
			if (_device->GetTransform(D3DTS_PROJECTION, &matrix) != D3D_OK) throw Error("get projection transform failed");
			return ConvertMatrix(matrix);
		}
        
		virtual System::Texture* createTexture(const char name[])
		{
			Texture *texture = new Texture(_device, name);
			addObject(texture);
			return texture;
		}

		virtual System::Mesh* createMesh(const std::vector<System::Vertex> &vertex, const std::vector<int> &index)
		{
			Mesh *mesh = new Mesh(_device, vertex, index);
			addObject(mesh);
			return mesh;
		}

		virtual System::Light* createLight()
		{
			Light *light = new Light();
			addObject(light);
			return light;
		}

		virtual System::Material* createMaterial()
		{
			Material *material = new Material();
			addObject(material);
			return material;
		}

		virtual System::Timer* createTimer()
		{
			Timer *timer = new Timer();
			addObject(timer);
			return timer;
		}

        virtual void selectTexture(System::Texture *texture, int stage)
		{
			if (texture)
			{
				// select texture

				assert(!_texture);

				_texture = (Texture*) texture;
				_texture->reference();
                				
				if (_device->SetTexture(stage, _texture->handle()) != D3D_OK) throw Error("set texture failed");
			}
			else
			{
				// unselect texture

				assert(_texture);

				if (_device->SetTexture(stage, 0) != D3D_OK) throw Error("disable texture failed");

				_texture->release();
				_texture = 0;
			}
		}

		virtual void selectLight(System::Light *light, int index)
		{
            if (_wireframe)
                return;

			assert(index>=0);
			assert(index<numberOfLights());

			if (light)
			{
				// setup light
				
				assert(!_lights[index]);

				_lights[index] = (Light*) light; 
				_lights[index]->reference();

                D3DLIGHT9 d3d_light = _lights[index]->light();

				if (_device->SetLight(index, &d3d_light) != D3D_OK) throw Error("set light failed");
				
				if (_device->LightEnable(index, TRUE) != D3D_OK) throw Error("enable light failed");
			}
			else
			{
				// disable light

				assert(_lights[index]);

				if (_device->LightEnable(index, FALSE) != D3D_OK) throw Error("disable light failed");

				_lights[index]->release();
				_lights[index] = 0;
			}
		}

		virtual void selectMaterial(System::Material *material)
		{
			if (material)
			{
				// setup material

				assert(!_material);

				_material = (Material*) material;
				_material->reference();

                D3DMATERIAL9 d3d_material = _material->material();

                if (_wireframe)
                {
                    d3d_material.Ambient.r = 0.7f;
                    d3d_material.Ambient.g = 0.7f;
                    d3d_material.Ambient.b = 0.7f;
                    d3d_material.Ambient.a = 1;
                }

				if (_device->SetMaterial(&d3d_material) != D3D_OK)
                    throw Error("set material failed");
			}
			else
			{
				// unselect material

				assert(_material);

				_material->release();
				_material = 0;
			}
		}

		virtual void selectMesh(System::Mesh *mesh)
		{
			if (mesh)
			{
				// select mesh

				assert(!_mesh);

				_mesh = (Mesh*) mesh; 
				_mesh->reference();

				if (_device->SetStreamSource(0, _mesh->vertex_buffer(), 0, sizeof(Vertex)) != D3D_OK) throw Error("set stream source failed");

				if (_device->SetIndices(_mesh->index_buffer()) != D3D_OK) throw Error("set indices failed");
			}
			else
			{
				// clear mesh

				assert(_mesh);

				if (_device->SetStreamSource(0, 0, 0, 0) != D3D_OK) throw Error("set stream source failed");

				if (_device->SetIndices(0) != D3D_OK) throw Error("set indices failed");

				_mesh->release();
				_mesh = 0;
			}
		}

		virtual void renderMesh()
		{
			assert(_mesh);

	        // draw indexed primitive

			Mesh *directx_mesh = (Mesh*) _mesh; 

			if (_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, directx_mesh->vertex_count(), 0, directx_mesh->triangle_count()) != D3D_OK) throw Error("draw indexed primitive failed");
		}

        virtual void blendMode(Blend::Mode mode)
        {
            switch (mode)
            {
                case Blend::NONE:
	                _device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                    break;

                case Blend::MULTIPLY:
	                _device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	                _device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
                    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
                    break;

                case Blend::ADD:
	                _device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	                _device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
                    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
                    break;

                case Blend::ALPHA:
	                _device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	                _device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
                    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
                    break;
            }
        }

        virtual void cullMode(Cull::Mode mode)
        {
            switch (mode)
            {
                case Cull::NONE:
                    _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
                    break;

                case Cull::CLOCKWISE:
                    _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
                    break;

                case Cull::COUNTERCLOCKWISE:
                    _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                    break;
            }
        }
        
        virtual void depthTest(bool enabled)
        {
            if (enabled)
            {
                _device->SetRenderState(D3DRS_ZENABLE, TRUE);
                _device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
            }
            else
            {
                _device->SetRenderState(D3DRS_ZENABLE, FALSE);
                _device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
            }
        }

        void beginQuadStream()
        {
            _quadEmitter.begin();
			
            _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        }

        void renderQuad(const Vector vertex[])
        {
            Vector *quad = _quadEmitter.quad();
            quad[0] = vertex[0];
            quad[1] = vertex[1];
            quad[2] = vertex[2];
            quad[3] = vertex[3];
        }

        void endQuadStream()
        {
            _quadEmitter.end();

            _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        }

        void beginLineStream()
        {
            _lineEmitter.begin();
			
            _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        }

        void renderLine(const Vector &a, const Vector &b)
        {
            Vector *line = _lineEmitter.line();
            line[0] = a;
            line[1] = b;
        }

        void endLineStream()
        {
            _lineEmitter.end();

            _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        }

        bool loadHeightfield(const char filename[], std::vector<float> &data, int &width, int &height)
        {
	        // get image info

	        D3DXIMAGE_INFO info;

	        if (FAILED(D3DXGetImageInfoFromFile(filename, &info)))
		        return false;
        	
	        width = info.Width;
	        height = info.Height;

	        // validate image format is acceptable

	        int bits = 0;

	        switch (info.Format)
	        {
		        case D3DFMT_A8R8G8B8:
			        System::log("ARBG8888");
			        bits = 32;
			        break;

		        case D3DFMT_X8R8G8B8:
			        System::log("XRGB8888");
			        bits = 32;
			        break;

		        case D3DFMT_R8G8B8:
			        System::log("RGB888");
			        bits = 24;
			        break;

		        case D3DFMT_R5G6B5:
			        System::log("RGB565");
			        bits = 16;
			        break;

		        case D3DFMT_X1R5G5B5:
			        System::log("RGB555");
			        bits = 16;
			        break;

		        case D3DFMT_A8:
			        System::log("A8");
			        bits = 8;
			        break;

		        case D3DFMT_P8:
			        System::log("P8");
			        bits = 8;
			        break;

		        case D3DFMT_L8:
			        System::log("L8");
			        bits = 8;
			        break;

		        // unsupported pixel format!

		        default:
                    System::log("unsupport format!");
			        return false;
	        }

	        // i *hate* manipulating 24bit image data...

	        if (bits==24)
		        info.Format = D3DFMT_A8R8G8B8;

	        // create surface

	        LPDIRECT3DSURFACE9 surface = 0;
            
	        if (FAILED(_device->CreateOffscreenPlainSurface(width, height, info.Format, D3DPOOL_SCRATCH, &surface, 0)))
		        return false;

	        // load heightfield image to surface

	        HRESULT result = D3DXLoadSurfaceFromFile(surface, 0, 0, filename, 0, D3DX_FILTER_NONE, 0, &info);

	        if (result==D3DERR_INVALIDCALL) printf("invalid call\n");
	        else if (result==D3DXERR_INVALIDDATA) printf("invalid data\n");

	        if (FAILED(result))
	        {
		        surface->Release();
		        return false;
	        }

	        // process surface into heightfield float samples

	        D3DLOCKED_RECT rect;
	        if (FAILED(surface->LockRect(&rect, 0, D3DLOCK_READONLY)))
	        {
		        surface->Release();
		        return false;
	        }

	        data.resize(width*height);

	        const void *image = rect.pBits;
	        unsigned int pitch = rect.Pitch;

	        if (bits==32)
	        {
                System::log("32 bpp");

		        for (int y=0; y<height; y++)
		        {
			        const unsigned int *src = (const unsigned int*) (((const unsigned char*)image) + y*pitch);
			        float *dst = &data[0] + width*y;

			        for (int x=0; x<width; x++)
			        {
				        dst[x] = ((float)(src[x]&0x00FFFFFF)) * (1.0f / 16777215);
				        assert(dst[x]>=0);
				        assert(dst[x]<=1);
			        }
		        }
	        }
	        else if (bits==24)
	        {
		        // we actually load it into 32 bit for ease of manipulation, but the actual data is still 24 bit

                System::log("24 bpp");

		        for (int y=0; y<height; y++)
		        {
			        const unsigned int *src = (const unsigned int*) (((const unsigned char*)image) + y*pitch);
			        float *dst = &data[0] + width*y;

			        for (int x=0; x<width; x++)
			        {
				        dst[x] = ((float)(src[x]&0x00FFFFFF)) * (1.0f / 16777215);
				        assert(dst[x]>=0);
				        assert(dst[x]<=1);
			        }
		        }
	        }
	        else if (bits==16)
	        {
                System::log("16 bpp");

		        for (int y=0; y<height; y++)
		        {
			        const unsigned short *src = (const unsigned short*) (((const unsigned char*)image) + y*pitch);
			        float *dst = &data[0] + width*y;

			        for (int x=0; x<width; x++)
			        {
				        dst[x] = ((float)src[x]) * (1.0f / 65535);
				        assert(dst[x]>=0);
				        assert(dst[x]<=1);
			        }
		        }
	        }
	        else if (bits==8)
	        {
                System::log("8 bpp");

		        for (int y=0; y<height; y++)
		        {
			        const unsigned char *src = ((const unsigned char*)image) + y*pitch;
			        float *dst = &data[0] + width*y;

			        for (int x=0; x<width; x++)
			        {
				        dst[x] = ((float)src[x]) * (1.0f / 255);
				        assert(dst[x]>=0);
				        assert(dst[x]<=1);
			        }
		        }
	        }
	        else
	        {
                System::log("unknown bpp");

		        for (unsigned int i=0; i<data.size(); i++)
			        data[i] = 0.5f;
	        }

	        surface->UnlockRect();

	        // release surface

	        surface->Release();

	        // success

            return true;
        }
    };
}
