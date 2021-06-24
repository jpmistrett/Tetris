#include "Core.h"
#include "Sprite.h"
#include "Vector2.h"
#include <time.h>
#include <list>

HMUSIC music;
HEFFECT fallEffect, lineEffect, gameoverEffect;
HTEXTURE tex;
float dx = 0.0f;
hgeSprite* spr;

const int height = 36; // 648 / 18;
const int width = 18; // 324 / 18;

bool rotate = 0; int colorNum = 1, gameRunning = true;

float hDelayBase = 0.1, delayBase = 0.3;
float hDelay = 0.1, delay = 0.3;
float timer = 0, hTimer = 0;
int linesHit = 0;

int field[height][width] = { 0 };

struct Point
{
	int x, y;
};

Point a[4], b[4];

int blockShapes[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
	//1,2,5,6, // S
};

void PlaySoundEffect(HEFFECT effect)
{
	g_hge->Effect_PlayEx(effect, 35, 1, 1);
}

void GameOver()
{
	if (gameRunning)
	{
		PlaySoundEffect(gameoverEffect);
		gameRunning = false;
	}
}

void RegenerateFallingBlock() 
{
	for (int i = 0; i < 4; i++)
	{
		field[b[i].y][b[i].x] = colorNum;
	}

	colorNum = 1 + rand() % 7;
	int n = rand() % 7;
	for (int i = 0; i < 4; i++)
	{
		a[i].x = blockShapes[n][i] % 2;
		a[i].y = blockShapes[n][i] / 2;
	}
}

bool CheckBounds()
{
	for (int i = 0; i < 4; i++) 
	{
		if (a[i].x < 0 || a[i].x >= width || a[i].y >= height) 
		{
			return 0;
		}
		else if (field[a[i].y][a[i].x])
		{
			return 0;
		}
	}
		
	return 1;
};

void ProcessMovementInput()
{
	if (g_hge->Input_GetKeyState(HGEK_LEFT))
	{
		hDelay = hDelayBase;
		dx = -1;
	}

	if (g_hge->Input_GetKeyState(HGEK_RIGHT))
	{
		hDelay = hDelayBase;
		dx = +1;
	}

	if (g_hge->Input_KeyDown(HGEK_UP))
	{
		rotate = true;
	}

	if (g_hge->Input_GetKeyState(HGEK_DOWN))
	{
		delay = 0.05;
	}
}

void Rotate()
{
	if (rotate)
	{
		Point p = a[1]; //center of rotation
		for (int i = 0; i < 4; i++)
		{
			int x = a[i].y - p.y;
			int y = a[i].x - p.x;
			a[i].x = p.x - x;
			a[i].y = p.y + y;
		}

		if (!CheckBounds())
		{
			for (int i = 0; i < 4; i++) a[i] = b[i];
		}

		rotate = false;
	}
}

void MoveSpriteXY()
{
	if (hTimer > hDelay)
	{
		for (int i = 0; i < 4; i++)
		{
			b[i] = a[i];
			a[i].x += dx;
		}

		if (!CheckBounds())
		{
			for (int i = 0; i < 4; i++)
			{
				if (a[i].y <= 0)
				{
					GameOver();
				}

				a[i] = b[i];
			}
		}
		hTimer = 0;
	}

	if (timer > delay)
	{
		for (int i = 0; i < 4; i++)
		{
			b[i] = a[i];
			a[i].y += 1;
		}

		if (!CheckBounds())
		{
			PlaySoundEffect(fallEffect);

			RegenerateFallingBlock();
		}

		timer = 0;
	}
}

void RemoveLines()
{
	int k = height - 1;

	for (int i = height - 1; i > 0; i--)
	{
		int count = 0;

		for (int j = 0; j < width; j++)
		{
			if (field[i][j])
			{
				count++;
			}

			field[k][j] = field[i][j];
		}

		if (count < width)
		{
			k--;
		}

		if (count == width)
		{
			PlaySoundEffect(lineEffect);

			//speed up v & h movement
			if (linesHit < 5)
			{
				hDelayBase -= 0.01;
				delayBase -= 0.05;
				linesHit++;
			}
		}
	}
}

void RenderSprites()
{
	//i=1 to remove top single block. Set to 0 to see root block
	for (int i = 1; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (field[i][j] == 0) continue;
			spr->SetTextureRect(field[i][j] * 18, 0, 18, 18);
			spr->Render(j * 18, i * 18);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		spr->SetTextureRect(colorNum * 18, 0, 18, 18);
		spr->Render(a[i].x * 18, a[i].y * 18);
	}
}

bool Update()
{
	if (!gameRunning) return 0;

	if (g_hge->Input_GetKeyState(HGEK_ESCAPE))
	{
		return 1;
	}

	timer += GetDeltaTime();
	hTimer += GetDeltaTime();

	ProcessMovementInput();

	MoveSpriteXY();

	Rotate();

	RemoveLines();

	dx = 0; rotate = false; delay = delayBase, hDelay = hDelayBase;

	return 0;
}

bool Render()
{	
	g_hge->Gfx_BeginScene();

	g_hge->Gfx_Clear(0);

	RenderSprites();

	g_hge->Gfx_EndScene();

	return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Initialize(324, 648, Update, Render);

	srand(time(0));

	if(IsInitialized())
	{
		gameoverEffect = g_hge->Effect_Load("resources/gameover.wav");
		fallEffect = g_hge->Effect_Load("resources/fall.wav");
		lineEffect = g_hge->Effect_Load("resources/line.wav");
		music = g_hge->Music_Load("resources/music.it");
		tex = g_hge->Texture_Load("resources/tiles.png");

		spr = new hgeSprite(tex, 0, 0, 18, 18);
		RegenerateFallingBlock();
	
		if (!music || !fallEffect || !lineEffect || !gameoverEffect || !tex)
		{
			Shutdown();
			return 0;
		}

		g_hge->Music_Play(music, true, 1);

		Run();

		g_hge->Texture_Free(tex);
		g_hge->Music_Free(music);
		g_hge->Effect_Free(fallEffect);
		g_hge->Effect_Free(lineEffect);
		g_hge->Effect_Free(gameoverEffect);
	}

	Shutdown();

	return 0;
}


