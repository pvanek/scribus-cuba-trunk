/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef COLOR_H
#define COLOR_H

#define TORGB( redVal, greenVal, blueVal ) \
{ \
	r = redVal; \
	g = greenVal; \
	b = blueVal; \
	return; \
}

void keywordToRGB( QString rgbColor, int &r, int &g, int &b )
{
	if( rgbColor == "aliceblue" )
		TORGB( 240, 248, 255)
	if( rgbColor == "antiquewhite" )
		TORGB( 250, 235, 215)
	if( rgbColor == "aqua" )
		TORGB( 0, 255, 255)
	if( rgbColor == "aquamarine" )
		TORGB( 127, 255, 212 )
	if( rgbColor == "azure" )
		TORGB( 240, 255, 255 )
	if( rgbColor == "beige" )
		TORGB( 245, 245, 220 )
	if( rgbColor == "bisque" )
		TORGB( 255, 228, 196 )
	if( rgbColor == "black" )
		TORGB( 0, 0, 0 )
	if( rgbColor == "blanchedalmond" )
		TORGB( 255, 235, 205 )
	if( rgbColor == "blue" )
		TORGB( 0, 0, 255 )
	if( rgbColor == "blueviolet" )
		TORGB( 138, 43, 226 )
	if( rgbColor == "brown" )
		TORGB( 165, 42, 42 )
	if( rgbColor == "burlywood" )
		TORGB( 222, 184, 135 )
	if( rgbColor == "cadetblue" )
		TORGB( 95, 158, 160 )
	if( rgbColor == "chartreuse" )
		TORGB( 127, 255, 0 )
	if( rgbColor == "chocolate" )
		TORGB( 210, 105, 30 )
	if( rgbColor == "coral" )
		TORGB( 255, 127, 80 )
	if( rgbColor == "cornflowerblue" )
		TORGB( 100, 149, 237 )
	if( rgbColor == "cornsilk" )
		TORGB( 255, 248, 220 )
	if( rgbColor == "crimson" )
		TORGB( 220, 20, 60 )
	if( rgbColor == "cyan" )
		TORGB( 0, 255, 255 )
	if( rgbColor == "darkblue" )
		TORGB( 0, 0, 139 )
	if( rgbColor == "darkcyan" )
		TORGB( 0, 139, 139 )
	if( rgbColor == "darkgoldenrod" )
		TORGB( 184, 134, 11 )
	if( rgbColor == "darkgray" )
		TORGB( 169, 169, 169 )
	if( rgbColor == "darkgrey" )
		TORGB( 169, 169, 169 )
	if( rgbColor == "darkgreen" )
		TORGB( 0, 100, 0 )
	if( rgbColor == "darkkhaki" )
		TORGB( 189, 183, 107 )
	if( rgbColor == "darkmagenta" )
		TORGB( 139, 0, 139 )
	if( rgbColor == "darkolivegreen" )
		TORGB( 85, 107, 47 )
	if( rgbColor == "darkorange" )
		TORGB( 255, 140, 0 )
	if( rgbColor == "darkorchid" )
		TORGB( 153, 50, 204 )
	if( rgbColor == "darkred" )
		TORGB( 139, 0, 0 )
	if( rgbColor == "darksalmon" )
		TORGB( 233, 150, 122 )
	if( rgbColor == "darkseagreen" )
		TORGB( 143, 188, 143 )
	if( rgbColor == "darkslateblue" )
		TORGB( 72, 61, 139 )
	if( rgbColor == "darkslategray" )
		TORGB( 47, 79, 79 )
	if( rgbColor == "darkslategrey" )
		TORGB( 47, 79, 79 )
	if( rgbColor == "darkturquoise" )
		TORGB( 0, 206, 209 )
	if( rgbColor == "darkviolet" )
		TORGB( 148, 0, 211 )
	if( rgbColor == "deeppink" )
		TORGB( 255, 20, 147 )
	if( rgbColor == "deepskyblue" )
		TORGB( 0, 191, 255 )
	if( rgbColor == "dimgray" )
		TORGB( 105, 105, 105 )
	if( rgbColor == "dimgrey" )
		TORGB( 105, 105, 105 )
	if( rgbColor == "dodgerblue" )
		TORGB( 30, 144, 255 )
	if( rgbColor == "firebrick" )
		TORGB( 178, 34, 34 )
	if( rgbColor == "floralwhite" )
		TORGB( 255, 250, 240 )
	if( rgbColor == "forestgreen" )
		TORGB( 34, 139, 34 )
	if( rgbColor == "fuchsia" )
		TORGB( 255, 0, 255 )
	if( rgbColor == "gainsboro" )
		TORGB( 220, 220, 220 )
	if( rgbColor == "ghostwhite" )
		TORGB( 248, 248, 255 )
	if( rgbColor == "gold" )
		TORGB( 255, 215, 0 )
	if( rgbColor == "goldenrod" )
		TORGB( 218, 165, 32 )
	if( rgbColor == "gray" )
		TORGB( 128, 128, 128 )
	if( rgbColor == "grey" )
		TORGB( 128, 128, 128 )
	if( rgbColor == "green" )
		TORGB( 0, 128, 0 )
	if( rgbColor == "greenyellow" )
		TORGB( 173, 255, 47 )
	if( rgbColor == "honeydew" )
		TORGB( 240, 255, 240 )
	if( rgbColor == "hotpink" )
		TORGB( 255, 105, 180 )
	if( rgbColor == "indianred" )
		TORGB( 205, 92, 92 )
	if( rgbColor == "indigo" )
		TORGB( 75, 0, 130 )
	if( rgbColor == "ivory" )
		TORGB( 255, 255, 240 )
	if( rgbColor == "khaki" )
		TORGB( 240, 230, 140 )
	if( rgbColor == "lavender" )
		TORGB( 230, 230, 250 )
	if( rgbColor == "lavenderblush" )
		TORGB( 255, 240, 245 )
	if( rgbColor == "lawngreen" )
		TORGB( 124, 252, 0 )
	if( rgbColor == "lemonchiffon" )
		TORGB( 255, 250, 205 )
	if( rgbColor == "lightblue" )
		TORGB( 173, 216, 230 )
	if( rgbColor == "lightcoral" )
		TORGB( 240, 128, 128 )
	if( rgbColor == "lightcyan" )
		TORGB( 224, 255, 255 )
	if( rgbColor == "lightgoldenrodyellow" )
		TORGB( 250, 250, 210 )
	if( rgbColor == "lightgray" )
		TORGB( 211, 211, 211 )
	if( rgbColor == "lightgrey" )
		TORGB( 211, 211, 211 )
	if( rgbColor == "lightgreen" )
		TORGB( 144, 238, 144 )
	if( rgbColor == "lightpink" )
		TORGB( 255, 182, 193 )
	if( rgbColor == "lightsalmon" )
		TORGB( 255, 160, 122 )
	if( rgbColor == "lightseagreen" )
		TORGB( 32, 178, 170 )
	if( rgbColor == "lightskyblue" )
		TORGB( 135, 206, 250 )
	if( rgbColor == "lightslategray" )
		TORGB( 119, 136, 153 )
	if( rgbColor == "lightslategrey" )
		TORGB( 119, 136, 153 )
	if( rgbColor == "lightsteelblue" )
		TORGB( 176, 196, 222 )
	if( rgbColor == "lightyellow" )
		TORGB( 255, 255, 224 )
	if( rgbColor == "lime" )
		TORGB( 0, 255, 0 )
	if( rgbColor == "limegreen" )
		TORGB( 50, 205, 50 )
	if( rgbColor == "linen" )
		TORGB( 250, 240, 230 )
	if( rgbColor == "magenta" )
		TORGB( 255, 0, 255 )
	if( rgbColor == "maroon" )
		TORGB( 128, 0, 0 )
	if( rgbColor == "mediumaquamarine" )
		TORGB( 102, 205, 170 )
	if( rgbColor == "mediumblue" )
		TORGB( 0, 0, 205 )
	if( rgbColor == "mediumorchid" )
		TORGB( 186, 85, 211 )
	if( rgbColor == "mediumpurple" )
		TORGB( 147, 112, 219 )
	if( rgbColor == "mediumseagreen" )
		TORGB( 60, 179, 113 )
	if( rgbColor == "mediumslateblue" )
		TORGB( 123, 104, 238 )
	if( rgbColor == "mediumspringgreen" )
		TORGB( 0, 250, 154 )
	if( rgbColor == "mediumturquoise" )
		TORGB( 72, 209, 204 )
	if( rgbColor == "mediumvioletred" )
		TORGB( 199, 21, 133 )
	if( rgbColor == "midnightblue" )
		TORGB( 25, 25, 112 )
	if( rgbColor == "mintcream" )
		TORGB( 245, 255, 250 )
	if( rgbColor == "mistyrose" )
		TORGB( 255, 228, 225 )
	if( rgbColor == "moccasin" )
		TORGB( 255, 228, 181 )
	if( rgbColor == "navajowhite" )
		TORGB( 255, 222, 173 )
	if( rgbColor == "navy" )
		TORGB( 0, 0, 128 )
	if( rgbColor == "oldlace" )
		TORGB( 253, 245, 230 )
	if( rgbColor == "olive" )
		TORGB( 128, 128, 0 )
	if( rgbColor == "olivedrab" )
		TORGB( 107, 142, 35 )
	if( rgbColor == "orange" )
		TORGB( 255, 165, 0 )
	if( rgbColor == "orangered" )
		TORGB( 255, 69, 0 )
	if( rgbColor == "orchid" )
		TORGB( 218, 112, 214 )
	if( rgbColor == "palegoldenrod" )
		TORGB( 238, 232, 170 )
	if( rgbColor == "palegreen" )
		TORGB( 152, 251, 152 )
	if( rgbColor == "paleturquoise" )
		TORGB( 175, 238, 238 )
	if( rgbColor == "palevioletred" )
		TORGB( 219, 112, 147 )
	if( rgbColor == "papayawhip" )
		TORGB( 255, 239, 213 )
	if( rgbColor == "peachpuff" )
		TORGB( 255, 218, 185 )
	if( rgbColor == "peru" )
		TORGB( 205, 133, 63 )
	if( rgbColor == "pink" )
		TORGB( 255, 192, 203 )
	if( rgbColor == "plum" )
		TORGB( 221, 160, 221 )
	if( rgbColor == "powderblue" )
		TORGB( 176, 224, 230 )
	if( rgbColor == "purple" )
		TORGB( 128, 0, 128 )
	if( rgbColor == "red" )
		TORGB( 255, 0, 0 )
	if( rgbColor == "rosybrown" )
		TORGB( 188, 143, 143 )
	if( rgbColor == "royalblue" )
		TORGB( 65, 105, 225 )
	if( rgbColor == "saddlebrown" )
		TORGB( 139, 69, 19 )
	if( rgbColor == "salmon" )
		TORGB( 250, 128, 114 )
	if( rgbColor == "sandybrown" )
		TORGB( 244, 164, 96 )
	if( rgbColor == "seagreen" )
		TORGB( 46, 139, 87 )
	if( rgbColor == "seashell" )
		TORGB( 255, 245, 238 )
	if( rgbColor == "sienna" )
		TORGB( 160, 82, 45 )
	if( rgbColor == "silver" )
		TORGB( 192, 192, 192 )
	if( rgbColor == "skyblue" )
		TORGB( 135, 206, 235 )
	if( rgbColor == "slateblue" )
		TORGB( 106, 90, 205 )
	if( rgbColor == "slategray" )
		TORGB( 112, 128, 144 )
	if( rgbColor == "slategrey" )
		TORGB( 112, 128, 144 )
	if( rgbColor == "snow" )
		TORGB( 255, 250, 250 )
	if( rgbColor == "springgreen" )
		TORGB( 0, 255, 127 )
	if( rgbColor == "steelblue" )
		TORGB( 70, 130, 180 )
	if( rgbColor == "tan" )
		TORGB( 210, 180, 140 )
	if( rgbColor == "teal" )
		TORGB( 0, 128, 128 )
	if( rgbColor == "thistle" )
		TORGB( 216, 191, 216 )
	if( rgbColor == "tomato" )
		TORGB( 255, 99, 71 )
	if( rgbColor == "turquoise" )
		TORGB( 64, 224, 208 )
	if( rgbColor == "violet" )
		TORGB( 238, 130, 238 )
	if( rgbColor == "wheat" )
		TORGB( 245, 222, 179 )
	if( rgbColor == "white" )
		TORGB( 255, 255, 255 )
	if( rgbColor == "whitesmoke" )
		TORGB( 245, 245, 245 )
	if( rgbColor == "yellow" )
		TORGB( 255, 255, 0 )
	if( rgbColor == "yellowgreen" )
		TORGB( 154, 205, 50 )
}

#endif
