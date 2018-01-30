#include "Text.h"
#include "WindowHandler.h"

Text::Text(std::string text, bool bold, bool kerning) {
	createSurface(text, bold, kerning);
}

Text::~Text() {
	SDL_FreeSurface(surface);
	surface = nullptr;
}

void Text::createSurface(std::string text, bool bold, bool kerning) {
	fontBitmap = nullptr;
	surface    = nullptr;

	// MAKE SURE THE TEXT STRING IS NOT BLANK
	if (!text.empty()) {
		fontBitmap = (bold ? SDL_LoadBMP("assets/Arial_bold.bmp") : SDL_LoadBMP("assets/Arial.bmp"));

		// MAKE SURE WE SUCCESSFULLY LOADED THE FONT BITMAP FILE
		if (fontBitmap != nullptr) {
			const int    asciiOffset      = 33;
			const int    asciiEnd         = 126;
			const int    charactersPerRow = 20;
			const int    numberOfRows     = 10;
			int          bpp              = fontBitmap->format->BytesPerPixel;
			int          characterWidth   = (fontBitmap->w / charactersPerRow);
			int          characterHeight  = (fontBitmap->h / numberOfRows);
			SDL_Surface* screenSurface    = WindowHandler::getInstance().getSurface();
			int          maxCharacters    = (screenSurface->w / characterWidth) - 1;
			int          maxSurfaceWidth  = (text.size() * characterWidth);
			
			// LIMIT THE SURFACE WIDTH IF THE TEXT STRING IS TOO LONG
			if ((int)text.size() > maxCharacters) {
				maxSurfaceWidth = (maxCharacters * characterWidth);
			}

			// CREATE AN EMPTY TEXT SURFACE TO HOLD THE CHARACTER GLYPHS
			surface = SDL_CreateRGBSurface(
				0, maxSurfaceWidth, characterHeight, 32,
				screenSurface->format->Rmask, screenSurface->format->Gmask, 
				screenSurface->format->Bmask, screenSurface->format->Amask
			);

			// MAKE SURE WE SUCCESSFULLY CREATED THE TEXT SURFACE
			if (surface != nullptr) {
				// FILL THE TEXT SURFACE WITH WHITE PIXELS
				memset(surface->pixels, 0xFF, (surface->pitch * surface->h));

				int      column, row;
				int      lastX = 0, minX, maxX, minWidth;
				Uint8    pixel;
				SDL_Rect source, destination;

				// ITERATE EACH CHARACTER IN THE TEXT STRING
				for (int i = 0; (i < (int)text.size() && i < maxCharacters); i++) {
					// VALID ASCII (33 - 126) ('!' - '~')
					if (((int)text[i] >= asciiOffset) && ((int)text[i] <= asciiEnd)) {
						column = ((int)text[i] - asciiOffset) % charactersPerRow;
						row    = ((int)text[i] - asciiOffset) / charactersPerRow;
					// INVALID ASCII - USE SPACE CHARACTER (' ')
					} else {
						column = 14; row = 4;
					}

					// FIND THE CHARACTER GLYPH POSITION IN THE FONT BITMAP:
					//		    X						   Y					   W			   H
					source = { (column * characterWidth), (row * characterHeight), characterWidth, characterHeight };

					// ENABLE KERNING
					if (kerning) {
						// USE A 10 PIXEL WIDTH FOR KERNED SPACE CHARACTERS
						if (text[i] == ' ') {
							minWidth = 10;
							minX     = (source.x * bpp);
						// CALCULATE KERNED WIDTH FOR ALL OTHER CHARACTERS
						} else {
							minX = INT_MAX, maxX = 0;

							// CHECK EACH PIXEL COLOR (3 BYTES PER PIXEL: RGB or BGR)
							for (int r = source.y; r < source.y + source.h; r++) {
								for (int c = (source.x * bpp); c < (source.x + source.w) * bpp; c++) {
									// GET THE FIRST PIXEL COLOR (R OR B based on platform)
									pixel = ((Uint8*)fontBitmap->pixels)[(r * fontBitmap->pitch) + c];

									// CHECK IF THE COLOR IS NOT WHITE
									if (pixel < 0xFF) {
										// GET THE MINIMUM (LEFT-SIDE) NON-WHITE X-POSITION
										if (c < minX) {
											minX = c;
										// GET THE MAXIMUM (RIGHT-SIDE) NON-WHITE X-POSITION
										} else if (c > maxX) {
											maxX = c;
										}
									}
								}
							}

							// CALCULATE THE ACTUAL VALID PIXEL WIDTH NEEDED TO DISPLAY THE CHARACTER GLYPH
							minWidth = (maxX - minX) / bpp;

							if (minWidth <= 0) {
								minWidth = characterWidth;
							}

							// ADD AN EXTRA PIXEL TO FIX DIVISION/ROUNDING ISSUES,
							// AND SO THAT EACH CHARACTER GETS A LITTLE SPACING (LOOKS NICER).
							minWidth++;
						}

						// SET THE CHARACTER POSITION IN THE TEXT SURFACE:
						//				X	   Y  W			H
						destination = { lastX, 0, minWidth, characterHeight };
						lastX += minWidth;

						// ONLY GET THE KERNED CHARACTER GLYPH (MINIMUM WIDTH NEEDED FOR NON-WHITE PIXELS)
						source.x += ((minX / bpp) - source.x);
						source.w = minWidth;
					// DISABLE KERNING
					} else {
						// SET THE CHARACTER POSITION IN THE TEXT SURFACE:
						//				X					  Y	 W				 H
						destination = { (i * characterWidth), 0, characterWidth, characterHeight };
					}
					
					// COPY/BLIT THE CHARACTER (GLYPH) TO THE TEXT SURFACE
					SDL_BlitSurface(fontBitmap, &source, surface, &destination);
				}
			}
		}
	}

	SDL_FreeSurface(fontBitmap);
	fontBitmap = nullptr;
}

SDL_Surface* Text::getSurface() {
	return surface;
}
