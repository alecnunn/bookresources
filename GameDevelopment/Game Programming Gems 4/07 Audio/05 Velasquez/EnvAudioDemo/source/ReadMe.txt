=====================
Chapter 7 Section: 5
=====================

Implementing an Environmental Audio Solution Using EAX and ZoomFX

Scott Velasquez, Gearbox Software
scottv@gearboxsoftware.com

Description:

The main purpose of this demo is to demonstrate how to use EAX listener and source properties, Creative's EAX Manager to dynamically retrieve EAX reverberation and obstuction/occlusion data, and to demonstrate how to use Sensaura's ZoomFX.

The building in the level (and the water if you go inside of it) demonstrate different environments and how they effect the final mix of sounds playing.  The water stream shows off ZoomFX's ability to create volumetric sound sources.  If ZoomFX is enabled you will see a wireframe box which represents one DirectSound buffer's ZoomFX bounding box.  This area is where ZoomFX creates its magic of giving the sound source a feeling of size.  If ZoomFX is not enabled or not supported by your sound card, you will see eight wireframe spheres which represent eight DirectSound buffers which are needed to cover the whole area.  This should give you a better understanding of how cool ZoomFX really is; One ZoomFX buffer instead of eight regular buffers is a very good savings in valueable sound buffers.  

Let me describe some of the files included:

demo_final.x: DirectX level geometry which is used to render the level and was imported into Creative's EAGLE application to create audio geometry.
demo_final.eal: Environmental Audio Library used Creative's EAGLE application 
demo_final.eam: Environmental Audio Mesh used by Creative's EAGLE application

Instructions:

The first thing you will notice when browsing the source for this demo is that the EAX SDK is not included.  I wasn't allowed to include this with my demo so you will need to download the SDK and set it up in the empty folders I've provided.  I built the demos with EAX 2.0 and EAX Manager.  Extract the EAX SDK into the EAX folder in the root directory and extract the EAX Manager files into the EAX Manager directory.

You will need to register as a developer on Creative's website to download the EAX\EAX Manager SDK's and their EAGLE application.  Creative's developer website is http://developer.creative.com.  Once you are logged in, click on Games on the left hand side of the page (You should see My Profile, Price List, Games, Music, and Portable Devices listed).  The Development Kits section is where you will find the EAX SDKs and under Tools you will find downloads for EAGLE and EAX Manager. 

Lastly, this code is by no means super effecient.  For example, some of the DirectSound calls I make aren't batch deferred which decreases remixing.  Also, I tried to keep the demo code to a minimum and only include functionality needed to demonstrate the items I've mentioned above.  



