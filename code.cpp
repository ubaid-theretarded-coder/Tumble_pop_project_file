#include <iostream> 
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x=1152;
int screen_y=896;

int currentframe=0;
float frametime=0.12f;
float timer=0.0f;

void display_level(RenderWindow &window,char **lvl,Texture &bgTex,Sprite &bgSprite,Texture &blockTexture,Sprite &blockSprite,Texture &slopeUpTex,Sprite &slopeUpSprite,Texture &slopeDownTex,Sprite &slopeDownSprite,const int height,const int width,const int cell_size,bool useSlopes=false)
{
	window.draw(bgSprite);
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			if(lvl[i][j]=='#'||lvl[i][j]=='['||lvl[i][j]==']')
			{
				blockSprite.setPosition(j*cell_size,i*cell_size);
				window.draw(blockSprite);
			}
			else if(useSlopes&&lvl[i][j]=='/')
			{
				slopeUpSprite.setPosition(j*cell_size,i*cell_size);
				window.draw(slopeUpSprite);
			}
			else if(useSlopes&&lvl[i][j]=='\\')
			{
				slopeDownSprite.setPosition(j*cell_size,i*cell_size);
				window.draw(slopeDownSprite);
			}
		}
	}
}
void loadLevel1(char **lvl,int width,int height)
{
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			lvl[i][j]=' ';
		}
	}
	for(int i=0;i<13;i++)
	{
		lvl[i][0]=']';
		lvl[i][width-1]='[';
	}
	for(int i=0;i<18;i++)
	{
		lvl[13][i]='#';
		lvl[0][i]='#';
	}
	for(int i=0;i<18;i++)
	{
		if(i==8||i==9||i==10)
		{
			continue;
		}
		lvl[4][i]='#';
	}
	for(int i=0;i<18;i++)
	{
		if(i==7||i==8||i==13||i==14)
		{
			continue;
		}
		lvl[7][i]='#';
	}
	for(int i=0;i<18;i++)
	{
		if(i==3||i==4||i==11||i==12)
		{
			continue;
		}
		lvl[10][i]='#';
	}
}
void loadLevel2(char **lvl,int width,int height)
{
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			lvl[i][j]=' ';
		}
	}
	for(int i=0;i<height;i++)
	{
		lvl[i][0]='#';
		lvl[i][width-1]='#';
	}
	for(int i=0;i<width;i++)
	{
		lvl[13][i]='#';
		lvl[0][i]='#';
	}
	const int maxjump=3;
	const int platnum=6;
	int plat_x[platnum];
	int plat_y[platnum];
	int plat_width[platnum];
	bool plat_is_left[platnum];
	int half_screen=1;
	for(int i=0;i<platnum;i++)
	{
		bool valid=false;
		int try_count=0;
		while(!valid&&try_count<1000)
		{
			try_count++;
			int x_new;
			bool is_left_side;
			if(half_screen%2!=0)
			{
				x_new=2+rand()%5;
				is_left_side=true;
				half_screen++;
			}
			else
			{
				x_new=12+rand()%5;
				is_left_side=false;
				half_screen++;
			}
			int y_new=4+rand()%8;
			int min_width=4+rand()%3;
			bool too_close=false;
			bool unreachable=false;
			for(int j=0;j<i;j++)
			{
				int new_left=x_new;
				int new_right=x_new+min_width-1;
				int old_left=plat_x[j];
				int old_right=plat_x[j]+plat_width[j]-1;
				int y_diff=y_new-plat_y[j];
				int y_abs_diff=abs(y_diff);
				bool same_side=(is_left_side==plat_is_left[j]);
				if(same_side)
				{
					if(y_abs_diff<=2)
					{
						if(new_left>old_right)
						{
							int gap=new_left-old_right+1;
							if(gap<2)
							{
								too_close=true;
								break;
							}
						}
						else if(new_right<old_left)
						{
							int gap=old_left-new_right+11;
							if(gap<2)
							{
								too_close=true;
								break;
							}
						}
						else
						{
							too_close=true;
							break;
						}
					}
					if(y_diff<0)
					{
						int height_diff=abs(y_diff);
						if(height_diff>3)
						{
							unreachable=true;
							break;
						}
					}
				}
			}
			if(!too_close&&!unreachable)
			{
				plat_x[i]=x_new;
				plat_y[i]=y_new;
				plat_width[i]=min_width;
				plat_is_left[i]=is_left_side;
				for(int k=0;k<min_width;k++)
				{
					if(x_new+k<width-1)
					{
						lvl[y_new][x_new+k]='#';
					}
				}
				valid=true;
			}
		}
	}
	int num_slopes=1;
	for(int s=0;s<num_slopes;s++)
	{
		int start_slope_x=6+rand()%6;
		int start_slope_y=3+rand()%4;
		int slope_length=3+rand()%8;
		int steepness=1+rand()%2;
		int direction=(rand()%2==0)?1:-1;
		for(int step=0;step<slope_length;step++)
		{
			int step_x=start_slope_x+(step*direction);
			int step_y=start_slope_y+(step*steepness);
			if(step_x>=1&&step_x<width-1&&step_y>=1&&step_y<13)
			{
				if(lvl[step_y][step_x]==' ')
				{
					if(direction==1)
					{
						lvl[step_y][step_x]='\\';
					}
					else
					{
						lvl[step_y][step_x]='/';
					}
				}
			}
		}
	}
}

void colision_x(char **lvl,float &player_x,float &player_y,const int cell_size,int &PHeight,int &PWidth,float velocity,int direction)
{
	float track_x=player_x;
	track_x+=(direction==-1?-velocity:(direction==1?velocity:track_x));
	char left_top=lvl[(int)(player_y)/cell_size][(int)(track_x)/cell_size];
	char left_mid=lvl[(int)(player_y+PHeight/2)/cell_size][(int)(track_x)/cell_size];
	char left_bottom=lvl[(int)(player_y+PHeight-1)/cell_size][(int)(track_x)/cell_size];
	char right_top=lvl[(int)(player_y)/cell_size][(int)(track_x+PWidth)/cell_size];
	char right_mid=lvl[(int)(player_y+PHeight/2)/cell_size][(int)(track_x+PWidth)/cell_size];
	char right_bottom=lvl[(int)(player_y+PHeight-1)/cell_size][(int)(track_x+PWidth)/cell_size];
	char mid_bottom=lvl[(int)(player_y+PHeight)/cell_size][(int)(track_x+PWidth/2)/cell_size];
	if(mid_bottom=='/'||mid_bottom=='\\')
	{
		int tile_x=(int)((track_x+PWidth/2)/cell_size);
		int tile_y=(int)((player_y+PHeight)/cell_size);
		float tileLeft=tile_x*cell_size;
		float tileTop=tile_y*cell_size;
		float localX=(track_x+PWidth/2)-tileLeft;
		float slopeY;
		if(mid_bottom=='/')
			slopeY=tileTop+(cell_size-localX);
		else
			slopeY=tileTop+localX;
		player_y=slopeY-PHeight;
		player_x=track_x;
		return;
	}
	if(direction==1)
	{
		if(right_top!='#'&&right_top!='['&&right_top!=']'&&right_mid!='#'&&right_mid!='['&&right_mid!=']'&&right_bottom!='#'&&right_bottom!='['&&right_bottom!=']')
		{
			player_x=track_x;
		}
	}
	else if(direction==-1)
	{
		if(left_top!='#'&&left_top!='['&&left_top!=']'&&left_mid!='#'&&left_mid!='['&&left_mid!=']'&&left_bottom!='#'&&left_bottom!='['&&left_bottom!=']')
		{
			player_x=track_x;
		}
	}
	if(player_x>995)
		player_x=995;
}

void player_gravity(char **lvl,float &offset_y,float &velocityY,bool &onGround,const float &gravity,float &terminal_Velocity,float &player_x,float &player_y,const int cell_size,int &Pheight,int &Pwidth,int direction,float velocity)
{
	onGround=false;
	offset_y=player_y+velocityY;
	char bottom_left_down=lvl[(int)(offset_y+Pheight)/cell_size][(int)(player_x)/cell_size];
	char bottom_right_down=lvl[(int)(offset_y+Pheight)/cell_size][(int)(player_x+Pwidth)/cell_size];
	char bottom_mid_down=lvl[(int)(offset_y+Pheight)/cell_size][(int)(player_x+Pwidth/2)/cell_size];
	int midX=(int)(player_x+Pwidth/2);
	int midY=(int)(offset_y+Pheight);
	int tile_x=midX/cell_size;
	int tile_y=midY/cell_size;
	for(int ty=(int)((player_y+Pheight)/cell_size);ty<=tile_y;ty++)
	{
		char tile=lvl[ty][tile_x];
		if(tile=='/'||tile=='\\')
		{
			float tileTop=ty*cell_size;
			float localX=midX-tile_x*cell_size;
			float slopeY=(tile=='/')?(tileTop+(cell_size-localX)):(tileTop+localX);
			player_y=slopeY-Pheight;
			velocityY=0;
			onGround=true;
			return;
		}
	}
	if(velocityY>=0&&(bottom_left_down=='#'||bottom_mid_down=='#'||bottom_right_down=='#'||bottom_left_down=='['||bottom_mid_down=='['||bottom_right_down=='['||bottom_left_down==']'||bottom_mid_down==']'||bottom_right_down==']'))
	{
		onGround=true;
		player_y=(int)((offset_y+Pheight)/cell_size)*cell_size-Pheight;
		velocityY=0;
	}
	else
	{
		player_y=offset_y;
		onGround=false;
		velocityY+=gravity;
		if(velocityY>=terminal_Velocity)
			velocityY=terminal_Velocity;
	}
	if(player_y<45)
		player_y=45;
}

void check_edge(char **lvl,float &ghost_x,float ghost_y,int &ghost_direction,const int cell_size)
{
	int next_x=(ghost_x+ghost_direction*67)/cell_size;
	int below_y=(ghost_y+96)/cell_size;
	char ground=lvl[below_y][next_x];
	if(ground==' ')
	{
		ghost_direction*=-1;
	}
}

bool can_jump_up(char **lvl,float enemy_x,float enemy_y,const int cell_size)
{
	int tileY=(enemy_y-64)/cell_size;
	int tileX=(enemy_x+48)/cell_size;
	if(tileY<0)
	{
		return false;
	}
	else
	{
		return lvl[tileY][tileX]=='#';
	}
}
void skeleton_gravity(char **lvl,float &skel_x,float &skel_y,float &velocityY,bool &onGround,const float gravity,const float terminal_Velocity,const int cell_size)
{
	onGround=false;
	float offset_y=skel_y+velocityY;
	int below_y=(offset_y+96)/cell_size;
	int left_x=skel_x/cell_size;
	int right_x=(skel_x+96)/cell_size;
	if(below_y>=14||left_x<0||left_x>=18||right_x<0||right_x>=18)
	{
		onGround=true;
		skel_y=(13*cell_size)-96;
		velocityY=0;
		return;
	}
	char bottom_left=lvl[below_y][left_x];
	char bottom_right=lvl[below_y][right_x];
	if(velocityY>=0&&(bottom_left=='#'||bottom_right=='#'))
	{
		onGround=true;
		skel_y=(below_y*cell_size)-96;
		velocityY=0;
	}
	else
	{
		skel_y=offset_y;
		velocityY+=gravity;
		if(velocityY>terminal_Velocity)
			velocityY=terminal_Velocity;
	}
}

bool gun_hits_ghost(int i,float ghost_x[],float ghost_y[],float gun_x,float gun_y,float gunwidth,float gunheight,float ghostwidth=96,float ghostheight=87)
{
	if(gun_x<ghost_x[i]+ghostwidth&&gun_x+gunwidth>ghost_x[i]&&gun_y<ghost_y[i]+ghostheight&&gun_y+gunheight>ghost_y[i])
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool gun_hits_skel(int i,float skel_x[],float skel_y[],float gun_x,float gun_y,float gunwidth,float gunheight,float skelwidth=90,float skelheight=105)
{
	if(gun_x<skel_x[i]+skelwidth&&gun_x+gunwidth>skel_x[i]&&gun_y<skel_y[i]+skelheight&&gun_y+gunheight>skel_y[i])
	{
		return true;
	}
	else
	{
		return false;
	}
}

void loadmenu(Texture &bg_text,Texture &menutex,Sprite &sp_play,Sprite &sp_char,Sprite &sp_quit,Sprite &sp_sel,Sprite &sp_menu,float menu_y[3],float &sel_x)
{
	bg_text.loadFromFile("Data/menu_bg.jpg");
	menutex.loadFromFile("Data/Selection.png");
	sp_menu.setTexture(bg_text);
	sp_play.setTexture(menutex);
	sp_char.setTexture(menutex);
	sp_quit.setTexture(menutex);
	sp_sel.setTexture(menutex);
	IntRect opt_play(408,202,409,111);
	IntRect opt_char(408,344,409,111);
	IntRect opt_quit(408,486,409,111);
	IntRect opt_sel(33,30,421,124);
	sp_play.setTextureRect(opt_play);
	sp_char.setTextureRect(opt_char);
	sp_quit.setTextureRect(opt_quit);
	sp_sel.setTextureRect(opt_sel);
	sp_menu.setPosition(-30,-200);
	sp_menu.setScale(1.5,1.47);
	sp_play.setPosition(360,400);
	sp_char.setPosition(360,540);
	sp_quit.setPosition(360,680);
	sp_play.setScale(0.85,0.85);
	sp_char.setScale(0.85,0.85);
	sp_quit.setScale(0.85,0.85);
	sp_sel.setScale(0.85,0.85);
	menu_y[0]=393;
	menu_y[1]=533;
	menu_y[2]=673;
	sel_x=354;
}

void menu_control(int &select_item,int &menu_state,bool &menu_up,bool &menu_down,bool &menu_enter,RenderWindow &window)
{
	bool up_pressed=Keyboard::isKeyPressed(Keyboard::Up);
	bool down_pressed=Keyboard::isKeyPressed(Keyboard::Down);
	bool enter_pressed=Keyboard::isKeyPressed(Keyboard::Enter);
	if(up_pressed&&!menu_up)
	{
		select_item--;
		if(select_item<0)
		{
			select_item=2;
		}
	}
	if(down_pressed&&!menu_down)
	{
		select_item++;
		if(select_item>=3)
		{
			select_item=0;
		}
	}
	if(enter_pressed&&!menu_enter)
	{
		if(select_item==0)
		{
			menu_state=1;
		}
		else if(select_item==1)
		{
			menu_state=2;
			select_item = 0;
		}
		else if(select_item==2)
		{
			window.close();
		}
	}
	menu_down=down_pressed;
	menu_up=up_pressed;
	menu_enter=enter_pressed;
}

void draw_menu(RenderWindow &window,Sprite &sp_menu,Sprite &sp_play,Sprite &sp_char,Sprite &sp_quit,Sprite &sp_sel,int select_item,float menu_y[3],float sel_x)
{
	window.clear();
	window.draw(sp_menu);
	window.draw(sp_play);
	window.draw(sp_char);
	window.draw(sp_quit);
	float pos_sel=menu_y[select_item];
	sp_sel.setPosition(sel_x,pos_sel);
	window.draw(sp_sel);
	window.display();
}
void load_textures(Texture &playerSingleTexture,Texture &playerSheetTexture,Texture &y_player,Texture &ghostSheetTexture,Texture &skelSheetTexture,Texture &bgTex,Texture &blockTexture,Texture &GunText,Texture &bg2Tex,Texture &block2Texture,Texture &slopeUpTexture,Texture &slopeDownTexture)
{
	playerSingleTexture.loadFromFile("Data/player.png");
	playerSheetTexture.loadFromFile("Data/Arcade - Tumble Pop - Playable Characters - Tumblepoppers.png");
	y_player.loadFromFile("Data/Arcade - Tumble Pop - Playable Characters - Tumblepoppers.png");
	ghostSheetTexture.loadFromFile("Data/Ghost.png");
	skelSheetTexture.loadFromFile("Data/skeleton.png");
	bgTex.loadFromFile("Data/bg.png");
	blockTexture.loadFromFile("Data/block1.png");
	GunText.loadFromFile("Data/Arcade - Tumble Pop - Playable Characters - Tumblepoppers.png");
	bg2Tex.loadFromFile("Data/bg2.png");
	block2Texture.loadFromFile("Data/block2.png");
	slopeUpTexture.loadFromFile("Data/slope_up.png");
	slopeDownTexture.loadFromFile("Data/slope_down.png");
}

void initial_sprites(Sprite &PlayerSprite,Sprite &ghostSprite,Sprite &skelSprite,Sprite &bgSprite,Sprite &blockSprite,Sprite &Gunsprite,Sprite &bg2Sprite,Sprite &block2Sprite,Sprite &slopeUpSprite,Sprite &slopeDownSprite,Texture &playerSingleTexture,Texture &ghostSheetTexture,Texture &skelSheetTexture,Texture &bgTex,Texture &blockTexture,Texture &GunText,Texture &bg2Tex,Texture &block2Texture,Texture &slopeUpTexture,Texture &slopeDownTexture)
{
	PlayerSprite.setTexture(playerSingleTexture);
	PlayerSprite.setScale(3,3);
	ghostSprite.setTexture(ghostSheetTexture);
	ghostSprite.setScale(3,3);
	skelSprite.setTexture(skelSheetTexture);
	skelSprite.setScale(3,3);
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);
	blockSprite.setTexture(blockTexture);
	Gunsprite.setTexture(GunText);
	bg2Sprite.setTexture(bg2Tex);
	block2Sprite.setTexture(block2Texture);
	slopeUpSprite.setTexture(slopeUpTexture);
	slopeDownSprite.setTexture(slopeDownTexture);
}

void handle_player_input(bool alive,char **lvl,float &player_x,float &player_y,int cell_size,int PlayerHeight,int PlayerWidth,float speed,int &directions,float &velocityY,bool onGround)
{
	if(alive)
	{
		if(Keyboard::isKeyPressed(Keyboard::Left))
		{
			colision_x(lvl,player_x,player_y,cell_size,PlayerHeight,PlayerWidth,speed,-1);
			directions=-1;
		}
		if(Keyboard::isKeyPressed(Keyboard::Right))
		{
			colision_x(lvl,player_x,player_y,cell_size,PlayerHeight,PlayerWidth,speed,1);
			directions=1;
		}
		if(Keyboard::isKeyPressed(Keyboard::Up)&&onGround)
		{
			velocityY=-18;
		}
	}
}

void check_player_enemy_collision(float player_x,float player_y,int PlayerWidth,int PlayerHeight,const int ghost_numb,float ghost_x[],float ghost_y[],bool ghostdead[],const int skelnumb,float skel_x[],float skel_y[],bool skeldead[],bool &alive,int &death_frame,float &death_timer)
{
	if(alive)
	{
		for(int i=0;i<ghost_numb;i++)
		{
			if(player_x+PlayerWidth>ghost_x[i]&&player_x<ghost_x[i]+(PlayerWidth-36)&&player_y<ghost_y[i]+66&&player_y+PlayerHeight>ghost_y[i]-22&&ghostdead[i]==false)
			{
				death_frame=0;
				alive=false;
				death_timer=0;
				break;
			}
		}
		for(int i=0;i<skelnumb;i++)
		{
			if(player_x<skel_x[i]+96&&player_x+PlayerWidth>skel_x[i]&&player_y<skel_y[i]+99&&player_y+PlayerHeight>skel_y[i]&&skeldead[i]==false)
			{
				alive=false;
				death_frame=0;
				death_timer=0;
				break;
			}
		}
	}
}

void player_movement(Sprite &PlayerSprite,Texture &playerSingleTexture,Texture &playerSheetTexture,bool alive,bool moving,bool onGround,float velocityY,int &death_frame,float &death_timer,float delay,int directions,float player_x,float player_y,IntRect life_end[],IntRect walkFrames[],IntRect jumpup[],IntRect jumpdown[],IntRect standframe=IntRect(0,0,32,34),float standy=3.47,float standiny=20)
{
	if(alive==false)
	{
		PlayerSprite.setTexture(playerSheetTexture);
		death_timer+=delay;
		if(death_timer>=0.15f)
		{
			death_timer=0;
			death_frame++;
			if(death_frame>=8)
			{
				death_frame=7;
			}
		}
		PlayerSprite.setTextureRect(life_end[death_frame]);
		if(directions==-1)
		{
			PlayerSprite.setScale(3,3);
			PlayerSprite.setPosition(player_x,player_y-44);
		}
		else
		{
			PlayerSprite.setScale(-3,3);
			PlayerSprite.setPosition(player_x+96,player_y-44);
		}
	}
	else if(moving&&onGround)
	{
		PlayerSprite.setTexture(playerSheetTexture);
		if(timer>=frametime)
		{
			timer=0;
			currentframe=(currentframe+1)%5;
			if(currentframe>=5)
			{
				currentframe=0;
			}
			PlayerSprite.setTextureRect(walkFrames[currentframe]);
		}
		PlayerSprite.setPosition(player_x,player_y-44);
		if(directions==-1)
		{
			PlayerSprite.setScale(3,3);
			PlayerSprite.setPosition(player_x,player_y-44);
		}
		else if(directions==1)
		{
			PlayerSprite.setScale(-3,3);
			PlayerSprite.setPosition(player_x+96,player_y-44);
		}
	}
	else if(!onGround)
	{
		currentframe=0;
		PlayerSprite.setTexture(playerSheetTexture);
		if(velocityY<0)
		{
			PlayerSprite.setTextureRect(jumpup[0]);
		}
		else if(velocityY>0)
		{
			PlayerSprite.setTextureRect(jumpdown[0]);
		}
		if(directions==-1)
		{
			PlayerSprite.setScale(3,3);
			PlayerSprite.setPosition(player_x,player_y);
		}
		else if(directions==1)
		{
			PlayerSprite.setScale(-3,3);
			PlayerSprite.setPosition(player_x+96,player_y);
		}
	}
	else
	{
		PlayerSprite.setTexture(playerSingleTexture);
		PlayerSprite.setTextureRect(standframe);
		currentframe=0;
		if(directions==-1)
		{
			PlayerSprite.setScale(3,standy);
			PlayerSprite.setPosition(player_x,player_y-standiny);
		}
		else if(directions==1)
		{
			PlayerSprite.setScale(-3,standy);
			PlayerSprite.setPosition(player_x+96,player_y-standiny);
		}
	}
}

void ghost_movement(const int ghost_numb,float ghost_x[],float ghost_y[],int ghost_direction[],bool ghost_is_stopped[],int ghost_frame[],float ghost_anim_timer[],float ghost_speed,float delay,char **lvl,int cell_size)
{
	for(int i=0;i<ghost_numb;i++)
	{
		ghost_anim_timer[i]+=delay;
		int random_chance=rand()%200;
		if(!ghost_is_stopped[i]&&random_chance==0)
		{
			ghost_is_stopped[i]=true;
			ghost_frame[i]=0;
		}
		else if(ghost_is_stopped[i]&&random_chance<10)
		{
			ghost_is_stopped[i]=false;
			ghost_frame[i]=0;
			if(rand()%2==0)
			{
				ghost_direction[i]*=-1;
			}
		}
		if(ghost_anim_timer[i]>=0.2)
		{
			ghost_anim_timer[i]=0;
			if(ghost_is_stopped[i])
			{
				ghost_frame[i]=(ghost_frame[i]+1)%2;
			}
			else
			{
				ghost_frame[i]=(ghost_frame[i]+1)%4;
			}
		}
		if(!ghost_is_stopped[i])
		{
			check_edge(lvl,ghost_x[i],ghost_y[i],ghost_direction[i],cell_size);
			ghost_x[i]+=ghost_speed*ghost_direction[i];
			if(ghost_x[i]<64||ghost_x[i]>1000)
			{
				ghost_direction[i]*=-1;
			}
		}
	}
}
void update_ghost_death(const int ghost_numb,bool ghostdead[],int ghost_death_frame[],float ghost_death_timer[],float ghost_x[],float ghost_y[],float player_x,float player_y,float delay)
{
	for(int i=0;i<ghost_numb;i++)
	{
		if(ghostdead[i]==true)
		{
			ghost_death_timer[i]+=delay;
			if(ghost_death_timer[i]>=0.1f)
			{
				ghost_death_timer[i]=0;
				if(ghost_death_frame[i]<5)
				{
					ghost_death_frame[i]+=1;
				}
			}
			if(ghost_x[i]<player_x)
			{
				ghost_x[i]+=4;
			}
			else if(ghost_x[i]>player_x)
			{
				ghost_x[i]-=4;
			}
			if(ghost_y[i]<player_y)
			{
				ghost_y[i]+=3;
			}
			else if(ghost_y[i]>player_y)
			{
				ghost_y[i]-=3;
			}
			if(ghost_death_frame[i]>=5)
			{
				ghost_x[i]=2000;
			}
		}
	}
}

void draw_ghosts(RenderWindow &window,Sprite &ghostSprite,const int ghost_numb,float ghost_x[],float ghost_y[],int ghost_direction[],bool ghostdead[],int ghost_death_frame[],bool ghost_is_stopped[],int ghost_frame[],IntRect Ghost_move[],IntRect Ghost_stop[],IntRect ghost_death[])
{
	for(int i=0;i<ghost_numb;i++)
	{
		if(!ghostdead[i])
		{
			if(ghost_is_stopped[i])
			{
				ghostSprite.setTextureRect(Ghost_stop[ghost_frame[i]]);
			}
			else
			{
				ghostSprite.setTextureRect(Ghost_move[ghost_frame[i]]);
			}
			if(ghost_direction[i]==-1)
			{
				ghostSprite.setScale(3,3);
				ghostSprite.setPosition(ghost_x[i],ghost_y[i]-24);
			}
			else
			{
				ghostSprite.setScale(-3,3);
				ghostSprite.setPosition(ghost_x[i]+96,ghost_y[i]-24);
			}
			window.draw(ghostSprite);
		}
		else
		{
			ghostSprite.setTextureRect(ghost_death[ghost_death_frame[i]]);
			if(ghost_direction[i]==1)
			{
				ghostSprite.setScale(-3,3);
			}
			else
			{
				ghostSprite.setScale(3,3);
			}
			ghostSprite.setPosition(ghost_x[i],ghost_y[i]-24);
			window.draw(ghostSprite);
		}
	}
}

void update_skeleton_logic(const int skelnumb,float skel_x[],float skel_y[],int skel_direction[],bool skel_is_stopped[],int skel_frame[],float skel_anim_timer[],float skel_speed,float skel_velocityY[],bool skel_onGround[],bool skel_prejump[],float skel_prejumptimer[],float delay,char **lvl,int cell_size,float gravity,float terminal_Velocity,const float prejump)
{
	for(int i=0;i<skelnumb;i++)
	{
		skel_anim_timer[i]+=delay;
		skeleton_gravity(lvl,skel_x[i],skel_y[i],skel_velocityY[i],skel_onGround[i],gravity,terminal_Velocity,cell_size);
		if(skel_onGround[i]&&rand()%100==0&&can_jump_up(lvl,skel_x[i],skel_y[i],cell_size))
		{
			skel_prejump[i]=true;
			skel_prejumptimer[i]=0.0;
			skel_is_stopped[i]=true;
			skel_velocityY[i]=0.0;
			skel_frame[i]=0;
		}
		if(skel_prejump[i])
		{
			skel_velocityY[i]=0;
			skel_prejumptimer[i]+=(delay);
			if(skel_prejumptimer[i]<0.30)
			{
				skel_frame[i]=0;
			}
			else if(skel_prejumptimer[i]<0.60)
			{
				skel_frame[i]=1;
			}
			else
			{
				skel_velocityY[i]=-16;
				skel_prejump[i]=false;
				skel_is_stopped[i]=false;
				skel_frame[i]=2;
				skel_prejumptimer[i]=0;
			}
		}
		int random_chance=rand()%200;
		if(!skel_is_stopped[i]&&random_chance==0)
		{
			skel_is_stopped[i]=true;
			skel_frame[i]=0;
		}
		else if(skel_is_stopped[i]&&random_chance<10)
		{
			skel_is_stopped[i]=false;
			skel_frame[i]=0;
			if(rand()%2==0)
			{
				skel_direction[i]*=-1;
			}
		}
		if(!skel_prejump[i])
		{
			if(skel_anim_timer[i]>=0.15)
			{
				skel_anim_timer[i]=0;
				if(skel_is_stopped[i]&&!skel_prejump[i])
				{
					skel_frame[i]=(skel_frame[i]+1)%3;
				}
				else if(skel_is_stopped[i]||skel_prejump[i])
				{
					skel_frame[i]=(skel_frame[i]+1)%3;
				}
				else
				{
					skel_frame[i]=(skel_frame[i]+1)%5;
				}
			}
		}
		if(!skel_is_stopped[i]&&!skel_prejump[i]&&skel_onGround[i])
		{
			skel_x[i]+=skel_speed*skel_direction[i];
			if(skel_x[i]<64||skel_x[i]>1000)
			{
				skel_direction[i]*=-1;
			}
		}
	}
}

void update_skeleton_death(const int skelnumb,bool skeldead[],int skel_death_frame[],float skel_death_timer[],float skel_x[],float skel_y[],float player_x,float player_y,float delay)
{
	for(int i=0;i<skelnumb;i++)
	{
		if(skeldead[i]==true)
		{
			skel_death_timer[i]+=delay;
			if(skel_death_timer[i]>=0.1f)
			{
				skel_death_timer[i]=0;
				if(skel_death_frame[i]<5)
				{
					skel_death_frame[i]+=1;
				}
			}
			if(skel_x[i]<player_x)
			{
				skel_x[i]+=4;
			}
			else if(skel_x[i]>player_x)
			{
				skel_x[i]-=4;
			}
			if(skel_y[i]<player_y)
			{
				skel_y[i]+=3;
			}
			else if(skel_y[i]>player_y)
			{
				skel_y[i]-=3;
			}
			if(skel_death_frame[i]>=5)
			{
				skel_x[i]=2000;
			}
		}
	}
}

void draw_skeletons(RenderWindow &window,Sprite &skelSprite,const int skelnumb,float skel_x[],float skel_y[],int skel_direction[],bool skeldead[],int skel_death_frame[],bool skel_is_stopped[],bool skel_onGround[],bool skel_prejump[],int skel_frame[],IntRect Skeleton_move[],IntRect skeleton_stop[],IntRect skeleton_jump[],IntRect skel_death[])
{
	for(int i=0;i<skelnumb;i++)
	{
		if(skeldead[i]==false)
		{
			if(skel_prejump[i]==true)
			{
				skelSprite.setTextureRect(skeleton_jump[skel_frame[i]]);
			}
			else if(skel_onGround[i]==false)
			{
				skelSprite.setTextureRect(skeleton_jump[2]);
			}
			else if(skel_is_stopped[i]==true)
			{
				skelSprite.setTextureRect(skeleton_stop[skel_frame[i]]);
			}
			else if(skel_is_stopped[i]==false)
			{
				skelSprite.setTextureRect(Skeleton_move[skel_frame[i]]);
			}
			if(skel_direction[i]==-1)
			{
				skelSprite.setScale(3,3);
				skelSprite.setPosition(skel_x[i],skel_y[i]-44);
			}
			else
			{
				skelSprite.setScale(-3,3);
				skelSprite.setPosition(skel_x[i]+96,skel_y[i]-44);
			}
			window.draw(skelSprite);
		}
		else
		{
			skelSprite.setTextureRect(skel_death[skel_death_frame[i]]);
			if(skel_direction[i]==1)
			{
				skelSprite.setScale(-3,3);
			}
			else
			{
				skelSprite.setScale(3,3);
			}
			skelSprite.setPosition(skel_x[i],skel_y[i]-44);
			window.draw(skelSprite);
		}
	}
}

void handle_gun_input(bool alive,bool &gun_active,int &gun_frame,float &gun_timer,bool &space_was_pressed)
{
	bool space_pressed_now=Keyboard::isKeyPressed(Keyboard::Space);
	if(alive&&space_pressed_now&&!space_was_pressed)
	{
		gun_active=true;
		gun_frame=0;
		gun_timer=0.0;
	}
	if(!space_pressed_now&&space_was_pressed)
	{
		gun_active=false;
		gun_frame=0;
	}
	space_was_pressed=space_pressed_now;
}

void update_gun_animation(bool gun_active,int &gun_frame,float &gun_timer,float gun_anim,float delay)
{
	if(gun_active)
	{
		gun_timer+=delay;
		if(gun_timer>=gun_anim)
		{
			gun_timer=0;
			if(gun_frame<3)
			{
				gun_frame++;
			}
		}
	}
}

void draw_gun_and_check_hits(RenderWindow &window,Sprite &Gunsprite,bool gun_active,int gun_frame,int directions,float player_x,float player_y,int PlayerWidth,IntRect gun[],const int ghost_numb,float ghost_x[],float ghost_y[],bool ghostdead[],int ghost_death_frame[],float ghost_death_timer[],const int skelnumb,float skel_x[],float skel_y[],bool skeldead[],int skel_death_frame[],float skel_death_timer[],int &scr,int enemy_type[],int enemy_id[],int &cap_count,int max_capacity)
{
	if(gun_active)
	{
		Gunsprite.setTextureRect(gun[gun_frame]);
		int frame_widths[4]={36,72,96,144};
		int frame_heights[4]={48,60,69,87};
		float gun_x_pos=player_x+PlayerWidth+frame_widths[gun_frame];
		float gun_x_neg=player_x-frame_widths[gun_frame];
		float gun_y=player_y+3;
		float gun_width=frame_widths[gun_frame];
		float gun_height=frame_heights[gun_frame];
		float gun_x_val;
		if(directions==1)
		{
			gun_x_val=gun_x_pos;
		}
		else if(directions==-1)
		{
			gun_x_val=gun_x_neg;
		}
		if(directions==1)
		{
			Gunsprite.setScale(-3,3);
			Gunsprite.setPosition(gun_x_pos,gun_y);
		}
		else
		{
			Gunsprite.setScale(3,3);
			Gunsprite.setPosition(gun_x_neg,gun_y);
		}
		for(int i=0;i<ghost_numb;i++)
		{
			if(!ghostdead[i]&&gun_hits_ghost(i,ghost_x,ghost_y,gun_x_val,gun_y,gun_width,gun_height))
			{
				if(cap_count<max_capacity)
				{
					enemy_type[cap_count]=0;
					enemy_id[cap_count]=i;
					cap_count++;
					ghostdead[i]=true;
					ghost_death_frame[i]=0;
					ghost_death_timer[i]=0;
					scr+=50;
				}
			}
		}
		for(int i=0;i<skelnumb;i++)
		{
			if(!skeldead[i]&&gun_hits_skel(i,skel_x,skel_y,gun_x_val,gun_y,gun_width,gun_height))
			{
				if(cap_count<max_capacity)
				{
					enemy_type[cap_count]=1;
					enemy_id[cap_count]=i;
					cap_count++;
					skeldead[i]=true;
					skel_death_frame[i]=0;
					skel_death_timer[i]=0;
					scr+=75;
				}
			}
		}
		window.draw(Gunsprite);
	}
}

string intToStr(int num){
	if(num==0)
	return "0";
	string result="";
	bool negative=false;
	if(num<0){
		negative=true;
		num=-num;
	}
	while(num>0){
		result=char('0'+(num%10))+result;
		num/=10;
	}
	if(negative)
	{
		result="-"+result;
	}
	return result;
}

void shoot_single_enemy(int enemy_type[],int enemy_id[],int &cap_count,float proj_x[],float proj_y[],int proj_direction[],int proj_type[],bool proj_active[],int proj_frame[],float proj_timer[],bool proj_rolling[],float player_x,float player_y,int gun_dir,int max_pro)
{
	if(cap_count<=0)return;
	int slot=-1;
	for(int i=0;i<max_pro;i++){
		if(!proj_active[i]){
			slot=i;
			break;
		}
	}
	if(slot==-1)return;
	cap_count--;
	proj_type[slot]=enemy_type[cap_count];
	proj_active[slot]=true;
	proj_frame[slot]=0;
	proj_timer[slot]=0;
	proj_rolling[slot]=false;
	proj_x[slot]=player_x+48;
	proj_y[slot]=player_y+20;
	proj_direction[slot]=gun_dir;
}

void shoot_all_enemies(int enemy_type[],int enemy_id[],int &cap_count,float proj_x[],float proj_y[],int proj_direction[],int proj_type[],bool proj_active[],int proj_frame[],float proj_timer[],bool proj_rolling[],float player_x,float player_y,int gun_dir,int max_pro)
{
	while(cap_count>0){
		shoot_single_enemy(enemy_type,enemy_id,cap_count,proj_x,proj_y,proj_direction,proj_type,proj_active,proj_frame,proj_timer,proj_rolling,player_x,player_y,gun_dir,max_pro);
	}
}

void update_projectiles(float proj_x[],float proj_y[],int proj_direction[],bool proj_active[],bool proj_rolling[],int proj_frame[],float proj_timer[],char** lvl,int cell_size,float delay,int max_pro)
{
	const float proj_speed_x=8.0f;
	const float proj_speed_y=10.0f;
	const float roll_speed=6.0f;
	const float gravity=1.0f;
	for(int i=0;i<max_pro;i++){
		if(!proj_active[i])continue;
		proj_timer[i]+=delay;
		if(proj_timer[i]>=0.1f){
			proj_timer[i]=0;
			proj_frame[i]=(proj_frame[i]+1)%8;
		}
		if(proj_rolling[i]){
			proj_x[i]+=roll_speed*proj_direction[i];
			int below_y=(proj_y[i]+96)/cell_size;
			int check_x=proj_x[i]/cell_size;
			if(below_y<14&&check_x>=0&&check_x<18){
				char ground=lvl[below_y][check_x];
				if(ground==' '){
					proj_rolling[i]=false;
				}
			}
		}else{
			if(proj_direction[i]==1){
				proj_x[i]+=proj_speed_x;
			}
			else if(proj_direction[i]==-1){
				proj_x[i]-=proj_speed_x;
			}
			else if(proj_direction[i]==2){
				proj_y[i]-=proj_speed_y;
			}
			else if(proj_direction[i]==-2){
				proj_y[i]+=proj_speed_y;
			}
			if(proj_direction[i]==1||proj_direction[i]==-1){
				proj_y[i]+=gravity*2;
			}
			int below_y=(proj_y[i]+96)/cell_size;
			int check_x=proj_x[i]/cell_size;
			if(below_y>=0&&below_y<14&&check_x>=0&&check_x<18){
				char ground=lvl[below_y][check_x];
				if(ground=='#'||ground=='['||ground==']'){
					proj_rolling[i]=true;
					proj_y[i]=(below_y*cell_size)-96;
					if(proj_direction[i]==2||proj_direction[i]==-2){
						proj_direction[i]=1;
					}
				}
			}
		}
		if(proj_x[i]<-100||proj_x[i]>1200||proj_y[i]<-100||proj_y[i]>900){
			proj_active[i]=false;
		}
	}
}

void draw_projectiles(RenderWindow &window,Sprite &ghostSprite,Sprite &skelSprite,float proj_x[],float proj_y[],int proj_type[],bool proj_active[],int proj_frame[],int proj_direction[],IntRect Ghost_thrown[],IntRect skel_thrown[],int max_pro)
{
	for(int i=0;i<max_pro;i++){
		if(!proj_active[i])continue;
		if(proj_type[i]==0){
			ghostSprite.setTextureRect(Ghost_thrown[proj_frame[i]]);
			if(proj_direction[i]>0){
				ghostSprite.setScale(-3,3);
				ghostSprite.setPosition(proj_x[i]+96,proj_y[i]);
			}else{
				ghostSprite.setScale(3,3);
				ghostSprite.setPosition(proj_x[i],proj_y[i]);
			}
			window.draw(ghostSprite);
		}
		else if(proj_type[i]==1){
			skelSprite.setTextureRect(skel_thrown[proj_frame[i]]);
			if(proj_direction[i]>0){
				skelSprite.setScale(-3,3);
				skelSprite.setPosition(proj_x[i]+96,proj_y[i]);
			}else{
				skelSprite.setScale(3,3);
				skelSprite.setPosition(proj_x[i],proj_y[i]);
			}
			window.draw(skelSprite);
		}
	}
}

int main()
{
	int lives=3;
	RenderWindow window(VideoMode(screen_x,screen_y),"Tumble-POP",Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	int scr=0;
	Font font;
	font.loadFromFile("score_font.ttf");
	Text score;
	score.setFont(font);
	score.setCharacterSize(40);
	score.setFillColor(Color::Cyan);
	score.setPosition(10,10);
	Texture bg_text;
	Texture menutex;
	Sprite sp_play,sp_char,sp_quit,sp_sel,sp_menu;
	float menu_y[3];
	float sel_x;
	int menu_state=0;
	int select_item=0;
	int char_selection=0;
	bool menu_up=false;
	bool menu_down=false;
	bool menu_enter=false;
	loadmenu(bg_text,menutex,sp_play,sp_char,sp_quit,sp_sel,sp_menu,menu_y,sel_x);
	Texture playerSingleTexture;
	Texture playerSheetTexture;
	srand(time(0));
	IntRect lifess[3]={{0,163,163,173},{0,163,340,173},{0,153,511,173}};
	Texture life;
	Sprite sp_lives;
	life.loadFromFile("Data/heart.png");
	IntRect walkFrames[5]={{16,33,32,48},{51,33,32,48},{84,33,32,48},{117,33,32,48},{150,33,32,48}};
	IntRect jumpup[1]={{354,33,32,48}};
	IntRect jumpdown[1]={{525,25,32,48}};
	IntRect life_end[8]={{20,82,32,48},{69,82,32,48},{118,82,32,48},{168,82,32,48},{217,82,32,48},{266,82,32,48},{310,82,32,48},{343,82,32,48}};
	IntRect gun[4]={{466,172,16,32},{433,172,32,32},{400,172,16,32},{349,172,48,32}};
	IntRect walkFrames_yell[5]={{16,222,32,48},{51,222,32,48},{84,222,32,48},{117,222,32,48},{150,222,32,48}};
	IntRect jumpup_yell[1]={{354,222,32,48}};
	IntRect jumpdown_yell[1]={{525,214,32,48}};
	IntRect life_end_yell[8]={{20,271,32,48},{69,271,32,48},{118,271,32,48},{168,271,32,48},{217,271,32,48},{266,271,32,48},{310,271,32,48},{343,271,32,48}};
	IntRect gun_yell[4]={{466,172,16,32},{433,172,32,32},{400,172,16,32},{349,172,48,32}};
	IntRect Ghost_move[4]={{8,8,48,32},{58,8,48,32},{108,8,48,32},{158,8,48,32}};
	IntRect Ghost_stop[2]={{214,8,48,32},{264,8,48,32}};
	const int ghost_numb=8;
	float ghost_x[ghost_numb]={150,700,200,600,967,700,300,800};
	float ghost_y[ghost_numb]={192,192,384,384,576,576,768,768};
	int ghost_direction[ghost_numb]={1,-1,1,-1,1,-1,1,-1};
	float ghost_speed=0.67;
	int ghost_frame[ghost_numb]={0,0,0,0,0,0,0,0};
	float ghost_anim_timer[ghost_numb]={0,0,0,0,0,0,0,0};
	bool ghost_is_stopped[ghost_numb]={false,false,false,false,false,false,false,false};
	Texture ghostSheetTexture;
	Sprite ghostSprite;
	IntRect Skeleton_move[5]={{8,24,32,48},{188,24,32,48},{222,24,32,48},{256,24,32,48},{290,24,32,48}};
	IntRect skeleton_stop[3]={{48,24,48,48},{98,24,48,48},{148,24,32,48}};
	IntRect skeleton_jump[3]={{536,24,32,48},{570,24,32,48},{604,24,32,48}};
	const int skelnumb=4;
	float skel_x[skelnumb]={150,700,300,800};
	float skel_y[skelnumb]={192,192,768,768};
	int skel_direction[skelnumb]={1,-1,1,-1};
	float skel_speed=1.67;
	int skel_frame[skelnumb]={0,0,0,0};
	float skel_anim_timer[skelnumb]={0,0,0,0};
	bool skel_is_stopped[skelnumb]={false,false,false,false};
	float skel_velocityY[skelnumb]={0,0,0,0};
	bool skel_onGround[skelnumb]={false,false,false,false};
	bool skel_prejump[skelnumb]={false,false,false,false};
	float skel_prejumptimer[skelnumb]={0,0,0,0};
	const float prejump=0.3;
	Texture skelSheetTexture;
	Sprite skelSprite;
	skelSheetTexture.loadFromFile("Data/skeleton.png");
	skelSprite.setTexture(skelSheetTexture);
	skelSprite.setScale(3,3);
	bool ghostdead[8]={false,false,false,false,false,false,false,false};
	int ghost_death_frame[8]={0,0,0,0,0,0,0,0};
	float ghost_death_timer[8]={0,0,0,0,0,0,0,0};
	bool skeldead[4]={false,false,false,false};
	int skel_death_frame[4]={0,0,0,0};
	float skel_death_timer[4]={0,0,0,0};
	IntRect ghost_death[6]={{632,8,48,32},{682,8,48,32},{738,8,48,32},{788,8,32,32},{822,8,32,32},{856,8,16,32}};
	IntRect Ghost_thrown[8]={{936,8,32,32},{970,8,32,32},{1004,8,32,32},{1038,8,32,32},{1072,8,32,32},{1106,8,32,32},{1140,8,32,32},{1174,8,32,32}};
	IntRect skel_death[6]={{768,24,32,48},{802,24,32,48},{842,24,32,48},{876,40,32,32},{910,40,48,32},{960,40,32,32}};
	IntRect skel_thrown[8]={{1040,40,32,32},{1074,40,32,32},{1108,40,32,32},{1142,40,32,32},{1176,40,32,32},{1210,40,32,32},{1244,40,32,32},{1278,40,32,32}};
	int current_level=1;
	const int max_cap_lvl1=3;
	const int max_cap_lvl2=5;
	int max_capacity=(current_level==1)?max_cap_lvl1:max_cap_lvl2;
	int enemy_type[5];
	int enemy_id[5];
	int cap_count=0;
	const int max_pro=5;
	float proj_x[max_pro];
	float proj_y[max_pro];
	int proj_direction[max_pro];
	int proj_type[max_pro];
	bool proj_active[max_pro];
	int proj_frame[max_pro];
	float proj_timer[max_pro];
	bool proj_rolling[max_pro];
	bool key_e_pressed=false;
	bool key_q_pressed=false;
	int gun_direction=0;
	for(int i=0;i<max_pro;i++)
	{
		proj_active[i]=false;
		proj_rolling[i]=false;
	}
	const int cell_size=64;
	const int height=14;
	const int width=18;
	char **lvl;
	Texture bgTex;
	Sprite bgSprite;
	Texture Player;
	Texture y_player;
	Texture y_player_stand;
	Sprite PlayerSprite;
	Texture blockTexture;
	Texture GunText;
	Texture blockleftTexture;
	Texture blockrightTexture;
	Sprite blockSprite;
	Sprite blockLeftSprite;
	Sprite blockRightSprite;
	Sprite Gunsprite;
	Texture bg2Tex;
	Texture block2Texture;
	Texture slopeUpTexture;
	Texture slopeDownTexture;
	Sprite bg2Sprite;
	Sprite spry_player_stand;
	Sprite spr_y_player;
	Sprite block2Sprite;
	Sprite slopeUpSprite;
	Sprite slopeDownSprite;
	load_textures(playerSingleTexture,playerSheetTexture,y_player,ghostSheetTexture,skelSheetTexture,bgTex,blockTexture,GunText,bg2Tex,block2Texture,slopeUpTexture,slopeDownTexture);
	initial_sprites(PlayerSprite,ghostSprite,skelSprite,bgSprite,blockSprite,Gunsprite,bg2Sprite,block2Sprite,slopeUpSprite,slopeDownSprite,playerSingleTexture,ghostSheetTexture,skelSheetTexture,bgTex,blockTexture,GunText,bg2Tex,block2Texture,slopeUpTexture,slopeDownTexture);
	int gun_frame=0;
	float gun_anim=0.15;
	float gun_timer=0;
	bool gun_active=false;
	bool space_was_pressed=false;
	Music lvlMusic;
	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);
	float player_x=500;
	float player_y=786;
	bool alive=true;
	int death_frame=0;
	float death_timer=0;
	float speed=5;
	const float jumpStrength=-20;
	const float gravity=1;
	bool isJumping=false;
	bool up_collide=false;
	bool left_collide=false;
	bool right_collide=false;
	bool onGround=false;
	float offset_x=0;
	float offset_y=0;
	float velocityY=0;
	float terminal_Velocity=20;
	int PlayerHeight=102;
	int PlayerWidth=96;
	bool up_button=false;
	char top_left='\0';
	char top_right='\0';
	char top_mid='\0';
	char left_mid='\0';
	char right_mid='\0';
	char bottom_left='\0';
	char bottom_right='\0';
	char bottom_mid='\0';
	char bottom_left_down='\0';
	char bottom_right_down='\0';
	char bottom_mid_down='\0';
	char top_right_up='\0';
	char top_mid_up='\0';
	char top_left_up='\0';
	y_player_stand.loadFromFile("Data/Arcade - Tumble Pop - Playable Characters - Tumblepoppers.png");
	playerSingleTexture.loadFromFile("Data/player.png");
	playerSheetTexture.loadFromFile("Data/Arcade - Tumble Pop - Playable Characters - Tumblepoppers.png");
	spry_player_stand.setTexture(y_player_stand);
	spry_player_stand.setTextureRect(walkFrames_yell[0]);
	spry_player_stand.setScale(3,3);
	PlayerSprite.setTexture(playerSingleTexture);
	PlayerSprite.setScale(3,3);
	lvl=new char *[height];
	for(int i=0;i<height;i+=1)
	{
		lvl[i]=new char[width];
	}
	loadLevel1(lvl,width,height);
	int directions=1;
	Event ev;
	while(window.isOpen())
	{
		while(window.pollEvent(ev))
		{
			if(ev.type==Event::Closed)
			{
				window.close();
			}
		}
		if(menu_state==0)
		{
			menu_control(select_item,menu_state,menu_up,menu_down,menu_enter,window);
			draw_menu(window,sp_menu,sp_play,sp_char,sp_quit,sp_sel,select_item,menu_y,sel_x);
			continue;
		}
		else if(menu_state==2)
		{
			if(Keyboard::isKeyPressed(Keyboard::Up)&&!menu_up)
			{
				char_selection=0;
			}
			if(Keyboard::isKeyPressed(Keyboard::Down)&&!menu_down)
			{
				char_selection=1;
			}
			if(Keyboard::isKeyPressed(Keyboard::Enter)&&!menu_enter)
			{
				menu_state=1;
			}
			if(Keyboard::isKeyPressed(Keyboard::Up))
			{
				menu_state=0;
			}
			menu_up=Keyboard::isKeyPressed(Keyboard::Up);
			menu_down=Keyboard::isKeyPressed(Keyboard::Down);
			menu_enter=Keyboard::isKeyPressed(Keyboard::Enter);
			window.clear();
			window.draw(sp_menu);
			PlayerSprite.setTexture(playerSingleTexture);
			PlayerSprite.setTextureRect(IntRect(0,0,32,34));
			PlayerSprite.setScale(2,2);
			PlayerSprite.setPosition(400,350);
			window.draw(PlayerSprite);
			spr_y_player.setTexture(y_player);
			spr_y_player.setTextureRect(walkFrames_yell[0]);
			spr_y_player.setScale(2,2);
			spr_y_player.setPosition(400,490);
			window.draw(spr_y_player);
			sp_sel.setPosition(354,char_selection==0?343:493);
			window.draw(sp_sel);
			window.display();
			continue;
		}
		handle_player_input(alive,lvl,player_x,player_y,cell_size,PlayerHeight,PlayerWidth,speed,directions,velocityY,onGround);
		handle_gun_input(alive,gun_active,gun_frame,gun_timer,space_was_pressed);
		if(Keyboard::isKeyPressed(Keyboard::W)){
			gun_direction=2;
		}
		else if(Keyboard::isKeyPressed(Keyboard::S)){
			gun_direction=-2;
		}
		else if(Keyboard::isKeyPressed(Keyboard::D)){
			gun_direction=1;
		}
		else if(Keyboard::isKeyPressed(Keyboard::A)){
			gun_direction=-1;
		}
		bool key_e_now=Keyboard::isKeyPressed(Keyboard::E);
		if(key_e_now&&!key_e_pressed&&cap_count>0){
			shoot_single_enemy(enemy_type,enemy_id,cap_count,proj_x,proj_y,proj_direction,proj_type,proj_active,proj_frame,proj_timer,proj_rolling,player_x,player_y,gun_direction,max_pro);
		}
		key_e_pressed=key_e_now;
		bool key_q_now=Keyboard::isKeyPressed(Keyboard::Q);
		if(key_q_now&&!key_q_pressed&&cap_count>0){
			shoot_all_enemies(enemy_type,enemy_id,cap_count,proj_x,proj_y,proj_direction,proj_type,proj_active,proj_frame,proj_timer,proj_rolling,player_x,player_y,gun_direction,max_pro);
		}
		key_q_pressed=key_q_now;
		if(Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		window.clear();
		if(current_level==1)
		{
			display_level(window,lvl,bgTex,bgSprite,blockTexture,blockSprite,slopeUpTexture,slopeUpSprite,slopeDownTexture,slopeDownSprite,height,width,cell_size,false);
		}
		else
		{
			display_level(window,lvl,bg2Tex,bg2Sprite,block2Texture,block2Sprite,slopeUpTexture,slopeUpSprite,slopeDownTexture,slopeDownSprite,height,width,cell_size,true);
		}
		player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity,player_x,player_y,cell_size,PlayerHeight,PlayerWidth,directions,speed);
		check_player_enemy_collision(player_x,player_y,PlayerWidth,PlayerHeight,ghost_numb,ghost_x,ghost_y,ghostdead,skelnumb,skel_x,skel_y,skeldead,alive,death_frame,death_timer);
		PlayerSprite.setPosition(player_x,player_y);
		bool moving=false;
		if(Keyboard::isKeyPressed(Keyboard::Right)||Keyboard::isKeyPressed(Keyboard::Left))
		{
			moving=true;
		}
		else
		{
			moving=false;
		}
		float delay=1.0f/60.0f;
		timer+=delay;
		if(char_selection==0)
		{
			player_movement(PlayerSprite,playerSingleTexture,playerSheetTexture,alive,moving,onGround,velocityY,death_frame,death_timer,delay,directions,player_x,player_y,life_end,walkFrames,jumpup,jumpdown);
			window.draw(PlayerSprite);
		}
		else
		{
			player_movement(spr_y_player,y_player_stand,playerSheetTexture,alive,moving,onGround,velocityY,death_frame,death_timer,delay,directions,player_x,player_y,life_end_yell,walkFrames_yell,jumpup_yell,jumpdown_yell,walkFrames_yell[0],3.0,44);
			window.draw(spr_y_player);
		}
		sp_lives.setTexture(life);
		sp_lives.setTextureRect(lifess[lives-1]);
		sp_lives.setScale(0.35,0.35);
		sp_lives.setPosition(960,1);
		window.draw(sp_lives);
		if(!alive&&death_frame>=7)
		{
			lives--;
			if(lives>0){
			player_x=100;
			player_y=594;
			alive=true;
			death_frame=0;
			death_timer=0;
			velocityY=0;
			}
			else{
				alive=false;
				window.close();
			}
		}
		ghost_movement(ghost_numb,ghost_x,ghost_y,ghost_direction,ghost_is_stopped,ghost_frame,ghost_anim_timer,ghost_speed,delay,lvl,cell_size);
		update_ghost_death(ghost_numb,ghostdead,ghost_death_frame,ghost_death_timer,ghost_x,ghost_y,player_x,player_y,delay);
		draw_ghosts(window,ghostSprite,ghost_numb,ghost_x,ghost_y,ghost_direction,ghostdead,ghost_death_frame,ghost_is_stopped,ghost_frame,Ghost_move,Ghost_stop,ghost_death);
		update_skeleton_logic(skelnumb,skel_x,skel_y,skel_direction,skel_is_stopped,skel_frame,skel_anim_timer,skel_speed,skel_velocityY,skel_onGround,skel_prejump,skel_prejumptimer,delay,lvl,cell_size,gravity,terminal_Velocity,prejump);
		update_skeleton_death(skelnumb,skeldead,skel_death_frame,skel_death_timer,skel_x,skel_y,player_x,player_y,delay);
		draw_skeletons(window,skelSprite,skelnumb,skel_x,skel_y,skel_direction,skeldead,skel_death_frame,skel_is_stopped,skel_onGround,skel_prejump,skel_frame,Skeleton_move,skeleton_stop,skeleton_jump,skel_death);
		int dead_count=0;
		for(int i=0;i<ghost_numb;i++){
			if(ghostdead[i])dead_count++;
		}
		for(int i=0;i<skelnumb;i++){
			if(skeldead[i])dead_count++;
		}
		if(dead_count==(ghost_numb+skelnumb)&&current_level==1){
			current_level=2;
			max_capacity=max_cap_lvl2;
			loadLevel2(lvl,width,height);
			for(int i=0;i<ghost_numb;i++){
				ghostdead[i]=false;
				ghost_death_frame[i]=0;
				ghost_death_timer[i]=0;
				ghost_x[i]=(i%2==0)?150+i*100:700-i*50;
				ghost_y[i]=192+i*70;
			}
			for(int i=0;i<skelnumb;i++){
				skeldead[i]=false;
				skel_death_frame[i]=0;
				skel_death_timer[i]=0;
				skel_x[i]=(i%2==0)?200+i*150:750-i*50;
				skel_y[i]=192+i*150;
			}
			player_x=100;
			player_y=594;
			velocityY=0;
			cap_count=0;
		}
		handle_gun_input(alive,gun_active,gun_frame,gun_timer,space_was_pressed);
		update_gun_animation(gun_active,gun_frame,gun_timer,gun_anim,delay);
		draw_gun_and_check_hits(window,Gunsprite,gun_active,gun_frame,directions,player_x,player_y,PlayerWidth,gun,ghost_numb,ghost_x,ghost_y,ghostdead,ghost_death_frame,ghost_death_timer,skelnumb,skel_x,skel_y,skeldead,skel_death_frame,skel_death_timer,scr,enemy_type,enemy_id,cap_count,max_capacity);
		update_projectiles(proj_x,proj_y,proj_direction,proj_active,proj_rolling,proj_frame,proj_timer,lvl,cell_size,delay,max_pro);
		draw_projectiles(window,ghostSprite,skelSprite,proj_x,proj_y,proj_type,proj_active,proj_frame,proj_direction,Ghost_thrown,skel_thrown,max_pro);
		score.setString("Score: "+intToStr(scr));
		window.draw(score);
		window.display();
	}
	lvlMusic.stop();
	for(int i=0;i<height;i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;
	return 0;
}