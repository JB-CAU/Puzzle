#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4819)

#include <bangtal>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

using namespace bangtal;

ScenePtr scene;
ObjectPtr game_board[36], game_answer[36]; // 최대 난이도로 맞춰서 선언
int piece_n = 3; // 퍼즐 크기가 nxn일 때의 n, 3이면 쉬움 6이면 보통.
int level = piece_n * piece_n; // 총 퍼즐 개수

int blank;

TimerPtr timer;
TimerPtr game_timer;
TimerPtr sec_timer;
float animationTime = 0.02f;
int game_time_limit = 1200;
int mixCount = 100;

int blank_index(ObjectPtr piece)
{
	for (int i = 0; i < level; i++)
	{
		if (game_board[i] == piece)
		return i;
	}
	return -1;
}

int index_to_x(int index)
{
	return (280 + (720/piece_n) * (index % piece_n));
}

int index_to_y(int index)
{
	return ((720-720/piece_n) - (720/piece_n) * (index / piece_n));
}

void game_move(int index)
{
	auto piece = game_board[index];
	game_board[index] = game_board[blank];
	game_board[index]->locate(scene, index_to_x(index), index_to_y(index));
	game_board[blank] = piece;
	game_board[blank]->locate(scene, index_to_x(blank), index_to_y(blank));
	blank = index;
}

bool is_movable(int index)
{
	if (blank % piece_n > 0 && index == blank - 1) return true;
	if (blank % piece_n < piece_n-1 && index == blank + 1)return true;
	if (blank / piece_n > 0 && index == blank - piece_n) return true;
	if (blank / piece_n < piece_n-1 && index == blank + piece_n) return true;
	return false;

}

int rand_move()
{
	int index = rand() % level;
	while (!is_movable(index)) {
		index = rand() % level;
	}
	return index;
}

bool check_end()
{
	for (int i = 0; i < level; i++)
	{
		if (game_board[i] != game_answer[i])
			return false;
	}
	return true;
}

void game_clear()
{
	char clear_message[200];
	sprintf(clear_message, "클리어하셨습니다! \n 클리어까지 총 %d초가 소요되었군요. 대단합니다.", 1200 - game_time_limit);
	showMessage(clear_message);
	game_timer->stop();
}

void game_over()
{
	showMessage("실패하셨습니다! \n 재도전하려면 다시하기 버튼을 눌러주세요.");
	game_timer->stop();
	sec_timer->stop();
}

void init_game()
{
	char path[30] = { 0, };

	if (level == 9) scene = Scene::create("게임판", "Images/조커_게임판.jpg");
	else if (level == 36) scene = Scene::create("게임판", "Images/스파이더맨_게임판.jpg");

	for (int i = 0; i < level; i++)
	{
		if (level == 9)
			sprintf(path, "Images/조커_%d.png", i);

		else if (level == 36)
			sprintf(path, "Images/스파이더맨_%d.png", i);

		game_board[i] = Object::create(path, scene, index_to_x(i), index_to_y(i));
		game_board[i]->setOnMouseCallback([&](auto piece, auto x, auto y, auto action)-> bool
			{
		
				int index = blank_index(piece);

				if (is_movable(index))
					game_move(index);

				if (check_end())
				{
					game_clear();
				}
					return true;
					});

		game_answer[i] = game_board[i];
	}
	
	blank = 7; 
	game_board[blank]->hide();

	game_timer = Timer::create(1200.f);

	auto restart = Object::create("Images/다시하기.png", scene, 1150, 0, true);
	restart->setScale(0.2f);
	restart->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		game_time_limit = 1200;
		mixCount = 100;
		timer->start();
		game_timer->set(1200.f);
		game_timer->start();
		sec_timer->start();
		return 0;
		});
}

int main()
{
	srand((unsigned int)time(NULL));
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	auto main = Scene::create("메인", "Images/메인.jpg");
	auto select = Scene::create("난이도 선택창", "Images/난이도 선택창 배경.png");
	bool diff_selected = false; // 난이도 선택을 이미 했는지
	int clear_time = 0; // 1초씩 감소시키면서 클리어까지 걸린 시간 체크
	char time_message[4] = { 0, };
	char time_finish[40] = "초 남기고 클리어하셨습니다!";
	
	auto game_start_button = Object::create("Images/게임시작버튼.png", select, 596, 44, false);
	game_start_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		init_game();
		timer->start(); 
		scene->enter();
		showTimer(game_timer);
		game_timer->start();
		sec_timer->start();
		return 0;
		});

	timer = Timer::create(animationTime);
	timer->setOnTimerCallback([&](auto)->bool
		{
			game_move(rand_move());
			mixCount--;
			if (mixCount > 0)
			{
				timer->set(animationTime);
				timer->start();
			}
			return true;
		});

	sec_timer = Timer::create(1.f);
	sec_timer->setOnTimerCallback([&](auto)->bool
		{
			game_time_limit--;
			if (game_time_limit > 0)
			{
				sec_timer->set(1.f);
				sec_timer->start();
			}
			else game_over();
			return true;
		});


	auto game_preview = Object::create("Images/조커_선택창.jpg", select, 512, 288, false);
	int temp_x = 0;
	int temp_y = 0;
	int i = 0;

	auto start_button = Object::create("Images/시작버튼.png", main, 1000, 20);
	start_button->setScale(0.5f);
	start_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		select->enter();
		return 0;
		});

	auto easy_button = Object::create("Images/난이도_쉬움.png", select, 66, 512);
	easy_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		game_preview->setImage("Images/조커_선택창.jpg");
		if (!diff_selected)
		{
			game_start_button->show();
			game_preview->show();
			diff_selected = true;
		}
		piece_n = 3;
		level = piece_n * piece_n;
		return 0;
		});

	
	auto normal_button = Object::create("Images/난이도_보통.png", select, 66, 305); // 퍼즐 만드는 과정은 생략
	normal_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		game_preview->setImage("Images/스파이더맨_선택창.jpg");
		if (!diff_selected)
		{
			game_start_button->show();
			game_preview->show();
			diff_selected = true;
		}
		piece_n = 6;
		level = piece_n * piece_n;
		return 0;
		});

	auto hard_button = Object::create("Images/난이도_어려움.png", select, 66, 97); // 추후 추가
	hard_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		game_preview->setImage("Images/어려움_선택창.png");
		if (!diff_selected)
		{
			game_start_button->show();
			game_preview->show();
			diff_selected = true;
		}
		return true;
		});

	startGame(main);
}
