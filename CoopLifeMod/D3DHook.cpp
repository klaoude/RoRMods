#include "D3DHook.h"
//#include "dhFastFont9.h"

void D3DHook::render()
{	
	// clear the window alpha
	m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	m_d3ddev->BeginScene();    // begins the 3D scene
	
	if (!m_mod) //if mod is disabled
	{
		m_d3ddev->EndScene();    // ends the 3D scene
		m_d3ddev->Present(NULL, NULL, NULL, NULL);
		return;
	}

	// select which vertex format we are using
	vHUD();
	m_d3ddev->SetFVF(CUSTOMFVF);

	// select the vertex buffer to display
	m_d3ddev->SetStreamSource(0, m_vbuffer, 0, sizeof(CUSTOMVERTEX));
	// copy the vertex buffer to the back buffer
	

	if (!m_solo && !m_isConnect) //draw howto if...
		DrawTextString(m_width / 2 - 150, m_height / 15, 100, 300, D3DCOLOR_ARGB(255, 255, 255, 255), "F1 to host | F2 to connect | F3 to dismiss", m_pFontDefault, DT_CENTER);

	if (m_err_life > 0) //Draw error if counter is still relevent
	{
		error();
		m_err_life--;
	}

	if (m_info_life > 0) //Draw info if counter is still relevent
	{
		info();
		m_info_life--;
	}

	if (m_mlife >= 0) //Draw HUD only if we're in game and not paused
	{
		if (!m_solo)
			m_d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_vertices.size() / 3);
		else
			m_stats.players[0].stats = m_plStats;
		textHud();
	}

	m_vbuffer->Release();
	m_d3ddev->EndScene();    // ends the 3D scene

	m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}

void D3DHook::initD3D(HWND hWnd)
{
	m_isConnect = false;
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;     // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = m_width;    // set the width of the buffer
	d3dpp.BackBufferHeight = m_height;    // set the height of the buffer

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp stuct
	m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_d3ddev);

	m_lmlife = LENGHT - 8*LENGHT /14;

	m_pSel = 0;

	vHUD(true);    // call the function to initialize the lifebar
	
	
	initFont(); //init font	
	setlmlife();

	////ipBox("");
}

void D3DHook::initFont()
{
	AddFontResourceEx("./Resources/RiskofRainSquare.ttf", FR_PRIVATE, 0); //add font to available resources
	AddFontResourceEx("./Resources/RiskofRainFont.ttf", FR_PRIVATE, 0); //add font to available resources

	D3DXCreateFont(m_d3ddev, 18, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainSquare", &m_pFont);
	D3DXCreateFont(m_d3ddev, 13, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainSquare", &m_pFontSmall);
	D3DXCreateFont(m_d3ddev, 22, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainFont", &m_pFontStat);
	D3DXCreateFont(m_d3ddev, 14, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainFont", &m_pFontNick);

	D3DXCreateFont(m_d3ddev, 13, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_pFontDefaultSmall);
	D3DXCreateFont(m_d3ddev, 17, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_pFontDefault);

	D3DXCreateFont(m_d3ddev, 12, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainSquare", &m_spFont);
	D3DXCreateFont(m_d3ddev, 13, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainFont", &m_spFontStat);


}

void D3DHook::vHUD(bool init /*=false*/)
{	
	if (m_stats.players.size() == 0)
		return;
	

	for (auto i = 0; i < m_stats.players.size(); i++) //fix life value somtimes going over maxhealth
		if (m_stats.players[i].stats.health > m_stats.players[i].stats.maxHealth)
			m_stats.players[i].stats.health = m_stats.players[i].stats.maxHealth;

	m_llives.clear();
	m_vertices.clear();

	m_lmlife = LENGHT - 2.5f * WIDTH / 14.0f;
	addRect(0, 0, 0, 0, D3DCOLOR_ARGB(0, 0, 0, 0)); //do not remove or world will end

	float yoff = WIDTH + 15.0f; //offset between 2 lifebars
	int j = 0; //lifebar id

	switch (m_scale) //set rect. pos. depending to scale
	{
		default: //TODO: set pos for each scale
	
			for (auto i=0; i < m_stats.players.size(); i++) //add as many lifebars outlines & backgrounds as needed
			{
				if (m_stats.players[i].pseudo == m_pseudo) continue; //skip our own lifebar (keeping right y index with j)

				addRect(7.0f, j * yoff + 100.0f, LENGHT, WIDTH, D3DCOLOR_ARGB(255, 64, 65, 87)); //EXTERNAL OUTLINE
				addRect(7.0f + WIDTH / 14.0f, j * yoff + 100.0f + WIDTH / 14.0f, LENGHT - 2.5f * WIDTH / 14.0f, WIDTH - 2.f * WIDTH / 14.f, D3DCOLOR_ARGB(255, 26, 26 , 26)); //HEALTH BACKGROUND
				j++;
			}

			refreshLife(init);
	}

	if (init) 	// create a vertex buffer interface called m_vbuffer, ONLY the 1st call of the function
		m_d3ddev->CreateVertexBuffer(m_vertices.size() * sizeof(CUSTOMVERTEX), NULL, CUSTOMFVF, D3DPOOL_MANAGED, &m_vbuffer, NULL);



	VOID* pVoid;	// a void pointer

	CUSTOMVERTEX array[100];
	std::copy(m_vertices.begin(), m_vertices.end(), array);	//creating array with content of m_vertices to ease cast in memcpy()
	
	m_vbuffer->Lock(0, 0, (void**)&pVoid, 0);	// lock m_vbuffer and load the vertices into it
	memcpy(pVoid, array, 20*m_vertices.size());
	m_vbuffer->Unlock();
}

void D3DHook::addRect(float x, float y, float l, float w, D3DCOLOR color)
{
	m_vertices.push_back({ x, y, 0.0f, 1.0f, color });	// top left
	m_vertices.push_back({ x + l, y, 0.0f, 1.0f, color });	//top right
	m_vertices.push_back({ x, y + w, 0.0f, 1.0f, color });	//bottom right

	m_vertices.push_back({ x + l, y, 0.0f, 1.0f, color });	// top right
	m_vertices.push_back({ x + l, y + w, 0.0f, 1.0f, color }); //bottom right
	m_vertices.push_back({ x, y + w, 0.0f, 1.0f, color });	//bottom left
}

void D3DHook::addLifeRect(float x, float y, float w, D3DCOLOR color, int player)
{
	if (m_llives.size() == 0) return;
	m_vertices.push_back({ x, y, 0.0f, 1.0f, color });	//top left
	m_vertices.push_back({ x + m_llives[player], y, 0.0f, 1.0f, color });	//top right
	m_vertices.push_back({ x + m_llives[player], y + w, 0.0f, 1.0f, color });	//bottom right

	m_vertices.push_back({ x + m_llives[player], y + w, 0.0f, 1.0f, color });	//bottom right
	m_vertices.push_back({ x, y + w, 0.0f, 1.0f, color }); //bottom left
	m_vertices.push_back({ x, y, 0.0f, 1.0f, color }); //top left
}

void D3DHook::refreshLife(bool init)
{

	int yoff = WIDTH + 15.0f; //y-axis offset

	m_llife = m_life * m_lmlife / m_mlife; //calc lenght of lifebar

	m_llives.clear();
	int j = 0;
	for (auto i = 0; i < m_stats.players.size(); i++) //calc all lenghts of player's lifebars
	{
		if (m_stats.players[i].pseudo == m_pseudo) continue; //skip our own lifebar, keeping right y offset with j
		m_llives.push_back(m_stats.players[i].stats.health * m_lmlife / m_stats.players[i].stats.maxHealth);
		j++;
	}

	if (!init) //remove old lifebars only if they exist 
		for (auto i = 0; i < 6*m_llives.size(); i++) //remove 6 vertices for each players (=> remove all lifebars)
			m_vertices.pop_back();

	//TODO: dependence on m_scale
	for (int i = 0; i < m_llives.size(); i++) //add as many lifebars as needed
	{
		if (m_llives[i] < 0.25*m_lmlife)
			addLifeRect(7.0f + WIDTH / 14.0f, i * yoff + 100.0f + WIDTH / 14.0f, WIDTH - 2.0f * WIDTH / 14.0f, D3DCOLOR_ARGB(255, 255, 0, 0), i); //HEALTH
		else
			addLifeRect(7.0f + WIDTH / 14.0f, i * yoff + 100.0f + WIDTH / 14.0f, WIDTH - 2.0f * WIDTH / 14.0f, D3DCOLOR_ARGB(255, 136, 211, 103), i); //HEALTH
	}


}

void D3DHook::textHud()
{


	int yoff = WIDTH +15.0f; //y offset of lifebars

	RECT container = { 0, 0, 0, 0 }; //container for outline

	std::ostringstream life[4], lvl[4], item, oItem[4];

	std::vector<std::string> pseudo;
	pseudo.clear();

	for (int i = 0; i < m_stats.players.size(); i++) //create array containing life and level strings to draw
	{
		switch (m_lifetext) //set lifetext format
		{
			case 0:
				life[i] << std::fixed << std::setprecision(0) << m_stats.players[i].stats.health << "/" << m_stats.players[i].stats.maxHealth;
				break;
			case 1: 
				life[i] << std::fixed << std::setprecision(0) << m_stats.players[i].stats.health;
				break;
			case 2:
				life[i] << std::fixed << std::setprecision(0) << m_stats.players[i].stats.maxHealth;
				break;
			case 3:
				life[i] << " ";
				break;
		}

		lvl[i] << "LV. " << std::fixed << std::setprecision(0) << m_stats.players[i].stats.level;
		pseudo.push_back(m_stats.players[i].pseudo);
		oItem[i] << std::fixed << std::setprecision(0) << m_stats.players[i].stats.item << " Items";

	}

	item << std::fixed << m_item << " ITEMS";

	int j = 0;

	//LIFE & OUTLINE
	if (!m_solo)
	{
		for (int i = 0; i < m_stats.players.size(); i++) //draw life and level of each existing player
		{
			if (m_stats.players[i].pseudo == m_pseudo) continue; //if we're trying to draw our own health & shit, skip but keep right y offset with j

			if (!m_sfont) //draw life with small or big font
			{
				DrawOutline(7.0f, 100.0f + j * yoff + WIDTH / 14, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 64, 64, 64), life[i].str().c_str(), m_pFont, DT_CENTER, &container, 1);
				DrawTextString(7.0f, 100.0f + j * yoff + WIDTH / 14, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 255), life[i].str().c_str(), m_pFont, DT_CENTER);
			}
			else
			{
				DrawOutline(7.0f, 100.0f + j * yoff + WIDTH / 14, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 64, 64, 64), life[i].str().c_str(), m_spFont, DT_CENTER, &container, 1);
				DrawTextString(7.0f, 100.0f + j * yoff + WIDTH / 14, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 255), life[i].str().c_str(), m_spFont, DT_CENTER);
			}

			DrawOutline(8, 100 + j * yoff + WIDTH, WIDTH, 4*LENGHT, D3DCOLOR_ARGB(255, 26, 26, 26), pseudo[i].c_str(), m_pFontNick, DT_LEFT, &container, 1);
			if (pseudo[i] == m_stats.players[m_pSel].pseudo) //if we're trying to draw the pseudo from the selected player
				DrawTextString(8, 100 + j * yoff + WIDTH, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 0), pseudo[i].c_str(), m_pFontNick, DT_LEFT, true);
			else
				DrawTextString(8, 100 + j * yoff + WIDTH, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 255), pseudo[i].c_str(), m_pFontNick, DT_LEFT, true);

			DrawOutline(11 + LENGHT  , 110 + j * yoff, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 26, 26, 26), lvl[i].str().c_str(), m_pFontSmall, DT_LEFT, &container, 1);
			if (pseudo[i] == m_stats.players[m_pSel].pseudo) //if we're trying to draw the level from the selected player
				DrawTextString(11 + LENGHT, 110 + j * yoff, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 0), lvl[i].str().c_str(), m_pFontSmall, DT_LEFT);
			else
				DrawTextString(11 + LENGHT, 110 + j * yoff, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 255), lvl[i].str().c_str(), m_pFontSmall, DT_LEFT);

			DrawOutline(11 + LENGHT, 100 + j * yoff, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 26, 26, 26), oItem[i].str().c_str(), m_pFontSmall, DT_LEFT, &container, 1);
			if (pseudo[i] == m_stats.players[m_pSel].pseudo) //if we're trying to draw the level from the selected player
				DrawTextString(11 + LENGHT, 100 + j * yoff, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 0), oItem[i].str().c_str(), m_pFontSmall, DT_LEFT);
			else
				DrawTextString(11 + LENGHT, 100 + j * yoff, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 255), oItem[i].str().c_str(), m_pFontSmall, DT_LEFT);

			j++;
		}
	}

	//ITEMS & OUTLINE
	DrawOutline(m_width / 2 - 85 - 78, m_height - 64 - 27 , WIDTH, 2 * LENGHT, D3DCOLOR_ARGB(255, 26, 26, 26), item.str().c_str(), m_pFontSmall, DT_LEFT, &container, 1);
	DrawTextString(m_width / 2 - 85 - 78, m_height - 64 - 27, WIDTH, 2 * LENGHT, D3DCOLOR_ARGB(255, 255, 255, 255), item.str().c_str(), m_pFontSmall, DT_LEFT);

	float height; //space between two stats
	if (!m_sfont)
		height = 18;
	else
		height = 13;

	std::ostringstream dmg, rate, crit, regen, strength, speed, leaf; //stats string

	dmg << "DMG:  " << std::fixed << std::setprecision(d_dmg) << m_stats.players[m_pSel].stats.damage;
	rate << "FIRERATE:  " << std::fixed << std::setprecision(d_rate) << m_stats.players[m_pSel].stats.attackSpeed;
	crit << "CRIT:  " << std::fixed << std::setprecision(d_crit) << m_stats.players[m_pSel].stats.critical << "%";
	regen << "REGEN:  " << std::fixed << std::setprecision(d_regen) << m_stats.players[m_pSel].stats.regeneration;
	strength << "STRENGTH:  " << std::fixed << std::setprecision(d_strength) << m_stats.players[m_pSel].stats.strength;
	speed << "SPEED: " << std::fixed << std::setprecision(d_speed) << m_stats.players[m_pSel].stats.speed;
	leaf << "DROP CHANCE: " << std::fixed << std::setprecision(d_leaf) << m_leaf << "%";

	if (!m_sfont)
	{
		if (m_iddmg > 0) DrawTextString(60.0f * m_width/100, 116.0f  + m_iddmg * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), dmg.str().c_str(),  m_pFontStat, DT_RIGHT);
		if (m_idrate > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idrate * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), rate.str().c_str(), m_pFontStat, DT_RIGHT);
		if (m_idcrit > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idcrit * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), crit.str().c_str(), m_pFontStat, DT_RIGHT);
		if (m_idregen > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idregen * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), regen.str().c_str(), m_pFontStat, DT_RIGHT);
		if (m_idstrength > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idstrength * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), strength.str().c_str(), m_pFontStat, DT_RIGHT);
		if (m_idspeed > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idspeed * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), speed.str().c_str(), m_pFontStat, DT_RIGHT);
		if (m_idleaf > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idleaf * height, height, 35.75 * m_width / 100, D3DCOLOR_ARGB(255, 192, 192, 192), leaf.str().c_str(), m_pFontStat, DT_RIGHT);
	}
	else
	{
		if (m_iddmg > 0) DrawTextString(60.0f * m_width/100, 116.0f  + m_iddmg * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), dmg.str().c_str(),  m_spFontStat, DT_RIGHT);
		if (m_idrate > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idrate * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), rate.str().c_str(), m_spFontStat, DT_RIGHT);
		if (m_idcrit > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idcrit * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), crit.str().c_str(), m_spFontStat, DT_RIGHT);
		if (m_idregen > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idregen * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), regen.str().c_str(), m_spFontStat, DT_RIGHT);
		if (m_idstrength > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idstrength * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), strength.str().c_str(), m_spFontStat, DT_RIGHT);
		if (m_idspeed > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idspeed * height, height, 35.75 * m_width/100, D3DCOLOR_ARGB(255, 192, 192, 192), speed.str().c_str(), m_spFontStat, DT_RIGHT);
		if (m_idleaf > 0) DrawTextString(60.0f * m_width / 100, 116.0f + m_idleaf * height, height, 35.75 * m_width / 100, D3DCOLOR_ARGB(255, 192, 192, 192), leaf.str().c_str(), m_spFontStat, DT_RIGHT);
	}
	


}

void D3DHook::DrawTextString(int x, int y, int h, int w, DWORD color, const char *str, LPD3DXFONT pfont, int align, bool calc/* = false*/)
{
	if (!calc) //draw text with preset rectangle
	{
		RECT container = { x, y, x + w, y + h };
		pfont->DrawText(NULL, str, -1, &container, align, color); //Output the text
		return;
	}

	//draw text while calc size of rect;
	RECT container = { x, y, 0, 0 };
	pfont->DrawText(NULL, str, -1, &container, DT_CALCRECT, 0);
	pfont->DrawText(NULL, str, -1, &container, align, color); //Output the text
	
}

void D3DHook::DrawOutline(int x, int y, int h, int w, DWORD color, const char *str, LPD3DXFONT pfont, int align, RECT *container, int size)
{	
	*container = { x - size, y, x + w, y + h }; // set container of text
	pfont->DrawText(NULL, str, -1, container, align, color); //left outline

	*container = { x + size, y, x + w, y + h };	// set container of text
	pfont->DrawText(NULL, str, -1, container, align, color);	//right outline

	*container = { x, y - size, x + w, y + h };	// set container of text
	pfont->DrawText(NULL, str, -1, container, align, color);	//top outline

	*container = { x, y + size, x + w, y + h };	// set container of text
	pfont->DrawText(NULL, str, -1, container, align, color); //bottom outline
}

void D3DHook::error()
{
	RECT Rect = { 10, m_height - 20,0,0 }; //set container pos

	m_pFontDefault->DrawText(NULL, m_error.c_str(), -1, &Rect, DT_CALCRECT, 0); //calc container size based on text
	m_pFontDefault->DrawText(NULL, m_error.c_str(), -1, &Rect, DT_LEFT, D3DCOLOR_ARGB(255, 255, 0, 0)); //draw text
}

void D3DHook::info()
{
	RECT Rect = { 10, m_height - 20,0,0 }; //set container pos

	m_pFontDefault->DrawText(NULL, m_info.c_str(), -1, &Rect, DT_CALCRECT, 0);	//calc container size based on text
	m_pFontDefault->DrawText(NULL, m_info.c_str(), -1, &Rect, DT_LEFT, D3DCOLOR_ARGB(255, 0, 255, 255));	//draw text
}

void D3DHook::setStat(Stats stats)
{
	setdmg(stats.damage);
	setrate(stats.attackSpeed);
	setcrit(stats.critical);
	setregen(stats.regeneration);
	setstrength(stats.strength);
	setitem(stats.item);
	setlvl(stats.level);
	setlife(stats.health);
	setmlife(stats.maxHealth);
}


void D3DHook::setDec(int dmg, int rate, int crit, int regen, int strength, int speed, int leaf)
{
	d_dmg = dmg;
	d_rate = rate;
	d_crit = crit;
	d_regen = regen;
	d_strength = strength;
	d_speed = speed;
	d_leaf = leaf;
}

void D3DHook::setId(int dmg, int rate, int crit, int regen, int strength, int speed, int leaf)
{
	m_iddmg = dmg;
	m_idrate = rate;
	m_idcrit = crit;
	m_idregen = regen;
	m_idstrength = strength;
	m_idspeed = speed;
	m_idleaf = leaf;
}


/*
void D3DHook::drawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char * fmt)
{
	RECT FontPos = { x, y, x + 120, y + 16 };
	char buf[1024] = { '\0' };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawText(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}
*/

/*
CUSTOMVERTEX vertices[] =
{
	//FIRST OUTLINE
	{ 10.0f, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },

	{ 10.0f, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },

	{ 10.0f, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },

	{ 10.0f + LENGHT, 100.0f , 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },

	//SECOND OUTLINE
	{ 10.0f + WIDTH / 16 , 100.0f + WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },
	{ 10.0f + LENGHT - WIDTH / 16, 100.0f + WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },
	{ 10.0f + WIDTH / 16, 100.0f + WIDTH - WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },

	{ 10.0f + LENGHT - WIDTH / 16 , 100.0f + WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },
	{ 10.0f + LENGHT - WIDTH / 16, 100.0f + WIDTH - WIDTH / 16 , 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },
	{ 10.0f + WIDTH / 16, 100.0f + WIDTH - WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },

	//BACKGROUND (NO-LIFE)
	{ 10.0f + 2 * WIDTH / 16 , 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },
	{ 10.0f + LENGHT - 2 * WIDTH / 16, 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },
	{ 10.0f + 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },

	{ 10.0f + LENGHT - 2 * WIDTH / 16 , 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },
	{ 10.0f + LENGHT - 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16 , 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },
	{ 10.0f + 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },

	//HEALTH (GREEN)
	{ 10.0f + 2 * WIDTH / 16 , 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },
	{ 10.0f + m_llife, 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },
	{ 10.0f + 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },

	{ 10.0f + m_llife , 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },
	{ 10.0f + m_llife, 100.0f + WIDTH - 2 * WIDTH / 16 , 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },
	{ 10.0f + 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },

};
*/

/*
void D3DHook::ipBox(const char *str)
{

float x = m_width/2, y = m_height / 15, l = 100, w = 13;
x -= l / 2;
D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 0, 0);

m_ipvertices.push_back({ x, y, 0.0f, 0.0f, color });
m_ipvertices.push_back({ x + l, y, 0.0f, 0.0f, color });
m_ipvertices.push_back({ x, y + w, 0.0f, 0.0f, color });

m_ipvertices.push_back({ x + l, y, 0.0f, 0.0f, color });
m_ipvertices.push_back({ x + l, y + w, 0.0f, 0.0f, color });
m_ipvertices.push_back({ x, y + w, 0.0f, 0.0f, color });


// create a vertex buffer interface called m_vbuffer
m_d3ddev->CreateVertexBuffer(m_ipvertices.size() * sizeof(CUSTOMVERTEX), NULL, CUSTOMFVF, D3DPOOL_MANAGED, &m_ipvbuffer, NULL);

VOID* pipVoid;    // a void pointer

CUSTOMVERTEX iparray[100];
std::copy(m_ipvertices.begin(), m_ipvertices.end(), iparray);
// lock m_vbuffer and load the vertices into it

m_ipvbuffer->Lock(0, 0, (void**)&pipVoid, 0);
memcpy(pipVoid, iparray, 20 * m_ipvertices.size());
m_ipvbuffer->Unlock();
}
*/
