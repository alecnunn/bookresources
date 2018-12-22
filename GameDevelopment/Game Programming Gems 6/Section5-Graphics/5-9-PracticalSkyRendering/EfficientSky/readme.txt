Title: Efficient Sky Demo

Created by: Aurelio Reis (AurelioReis@gmail.com) - www.CodeFortress.com

Version 1.0 (07/25/05):
Initial implementation.

Description:
This demo presents some of the key concepts discussed in the article
"Practical Sky Rendering for Games" in Game Gems 6.

For an up-to-date version of this demo please visit www.CodeFortress.com.

Features:
* Real-time dynamic sky calculated based on the current time of day.
* Sky Cube Rendering: cache the results of a sky render into a Cube Map.
* Per-pixel versus per-vertex sky implementation comparisions (for low, medium,
and high polygon sky dome representations).
* Normal Mapped Terrain.
* Sun Projection into the Sky.
* Volumetric Light Scattering (currently disabled).
* Star Field, pre-baked into a Cube Map and projected into the sky at night.
* Moon projection and moon light casting on the scene.

Wish List:
* Clouds, either through perlin noise or pre-generated scrolling fields.
* Projecting cloud image into the terrain for cloud shadows and/or using
high-resolution shadow maps for terrain shadows.
* Basic fog is in but getting scattering fog working again would be nice,
or perhaps another decent kind of aerial perspective technique.	
* Glare/Glow for the Sun and Moon through Post-Process Effects.

Fixmes:
* None.

Usage:
Use WASD to move and mouse button 1 to look around.

Requirements:
Win9x/WinNT/WinXP
D3D 9.0 compatible graphics card

Known Issues:
* Strange artifacts on Sky Cube in windowed mode.
* Seams on the low-res sky dome model's normal map.

Acknowledgments:
The algorithms presented here were inspired or based on a number of peoples work,
too many to list. Notable authors are Jensen, Preetham, and Nishita who have
contributed greatly to the study of dynamic skies in computer graphics.

Copyright and Disclaimer:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if
not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA