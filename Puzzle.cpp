#include <bangtal>
#include <windows.h>
#include <time.h>

using namespace bangtal;

struct puzzle {
	int x;
	int y;
	ObjectPtr puzzle_piece;
};


// 클릭한 위치가 빈 칸 상하좌우에 위치하였는지 체크

bool lcheck(int x, int y, struct puzzle* p, int blank_index)
{
	return (p[blank_index].x - 320 < x && x < p[blank_index].x && p[blank_index].y < y && y < p[blank_index].y + 240);
}

bool rcheck(int x, int y, struct puzzle* p, int blank_index)
{
	return (p[blank_index].x + 320 < x && x < p[blank_index].x+640 && p[blank_index].y < y && y < p[blank_index].y + 240);
}

bool upcheck(int x, int y, struct puzzle* p, int blank_index)
{
	return (p[blank_index].x < x && x < p[blank_index].x+320 && p[blank_index].y+240 < y && y < p[blank_index].y + 480);
}

bool downcheck(int x, int y, struct puzzle* p, int blank_index)
{
	return (p[blank_index].x  < x && x < p[blank_index].x+320 && p[blank_index].y-240 < y && y < p[blank_index].y);
}

bool puzzle_finish()
{
	return false;
}

int main()
{
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	auto main = Scene::create("메인", "Images/메인.jpg");
	auto select = Scene::create("난이도 선택창", "Images/난이도 선택창 배경.png");
	auto game_easy = Scene::create("게임창", "Images/빈칸.png");
	auto game_normal = Scene::create("게임창", "Images/스파이더맨_원본.jpg");
	// auto game_hard = Scene::create("게임창", "Images/스파이더맨_선택창.jpg");
	bool diff_selected = false; // 난이도 선택을 이미 했는지

	struct puzzle p[12] = {
		{ -480, -240, Object::create("Images/원본_20.png", game_easy, -480, -240) },
		{ -480, 0, Object::create("Images/원본_10.png", game_easy, -480, 0)},
		{ -480, 240, Object::create("Images/원본_00.png", game_easy, -480, 240)},
		{ -160, -240, Object::create("Images/원본_21.png", game_easy, -160, -240) },
		{ -160, 0, Object::create("Images/원본_11.png", game_easy, -160, 0)},
		{ -160, 240, Object::create("Images/원본_01.png", game_easy, -160, 240)},
		{ 160, -240, Object::create("Images/원본_22.png", game_easy, 160, -240) },
		{ 160, 0, Object::create("Images/원본_12.png", game_easy, 160, 0)},
		{ 160, 240, Object::create("Images/원본_02.png", game_easy, 160, 240)},
		{ 480, -240, Object::create("Images/원본_23.png", game_easy, 480, -240) },
		{ 480, 0, Object::create("Images/원본_13.png", game_easy, 480, 0) },
		{ 480, 240, Object::create("Images/원본_03.png", game_easy, 480, 240) }
	};

	auto timer = Timer::create(1200.f);

	auto game_start_button = Object::create("Images/게임시작버튼.png", select, 596, 44, false);
	auto game_preview = Object::create("Images/조커_선택창.jpg", select, 512, 288, false);
	int temp_x = 0;
	int temp_y = 0;
	int i = 0;

	auto restart = Object::create("Images/다시하기.png", game_easy, 650, -200, true);
	restart->setScale(0.2f);
	restart->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		while (i < 12) // 셔플
		{
			int rand_num = rand() % 12;

			temp_x = p[i].x;
			temp_y = p[i].y;
			p[i].puzzle_piece->locate(game_easy, p[rand_num].x, p[rand_num].y);
			p[i].x = p[rand_num].x;
			p[i].y = p[rand_num].y;
			p[rand_num].puzzle_piece->locate(game_easy, temp_x, temp_y);
			p[rand_num].x = temp_x;
			p[rand_num].y = temp_y;

			i++;
		}
		p[0].puzzle_piece->setImage("Images/빈 조각.png");
		int blank_index = 0;
		main->enter();
		return 0;
		});

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
		game_start_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			game_easy->enter();
			Sleep(1000);
			showTimer(timer);
			timer->start();
			return 0;
			});
		return 0;
		});

	srand(time(NULL));

	while (i < 12) // 셔플
	{
		int rand_num = rand() % 12;

			temp_x = p[i].x;
			temp_y = p[i].y;
			p[i].puzzle_piece->locate(game_easy, p[rand_num].x, p[rand_num].y);
			p[i].x = p[rand_num].x;
			p[i].y = p[rand_num].y;
			p[rand_num].puzzle_piece->locate(game_easy, temp_x, temp_y);
			p[rand_num].x = temp_x;
			p[rand_num].y = temp_y;

		i++;
	}
	p[0].puzzle_piece->setImage("Images/빈 조각.png");
	int blank_index = 0;

	/*
	 p[k].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		  if (lcheck(p[k].x + x, p[k].y + y, p, blank_index) || rcheck(p[k].x + x, p[k].y + y, p, blank_index) || upcheck(p[k].x + x, p[k].y + y, p, blank_index) || downcheck(p[k].x + x, p[k].y + y, p, blank_index))
				  {
					  temp_x = p[k].x;
					  temp_y = p[k].y;
					  p[k].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
					  p[k].x = p[blank_index].x;
					  p[k].y = p[blank_index].y;
					  p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
					  p[blank_index].x = temp_x;
					  p[blank_index].y = temp_y;
				  }
					  return 0;
				  });	
				  */
		

		p[0].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {

			printf("%d %d", p[0].x + x, p[0].y + y);
			if (lcheck(p[0].x + x, p[0].y + y, p, blank_index))
			{
				p[0].x += 320;
				p[blank_index].x -= 320;
				p[0].puzzle_piece->locate(game_easy, p[0].x, p[0].y);
				p[blank_index].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				blank_index = 0;
			}
			else if (rcheck(p[0].x + x, p[0].y + y, p, blank_index))
			{
				p[0].x -= 320;
				p[blank_index].x += 320;
				p[0].puzzle_piece->locate(game_easy, p[0].x, p[0].y);
				p[blank_index].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				blank_index = 0;
			}
			else if (upcheck(p[0].x + x, p[0].y + y, p, blank_index))
			{
				p[0].y -= 240;
				p[blank_index].y += 240;
				p[0].puzzle_piece->locate(game_easy, p[0].x, p[0].y);
				p[blank_index].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				blank_index = 0;
			}
			else if (downcheck(p[0].x + x, p[0].y + y, p, blank_index))
			{
				p[0].y += 240;
				p[blank_index].y -= 240;
				p[0].puzzle_piece->locate(game_easy, p[0].x, p[0].y);
				p[blank_index].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				blank_index = 0;
			}
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
		game_start_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			game_normal->enter();
			showTimer(timer);
			return 0;
			});
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
		 /* 난이도 쉬움과 보통을 클리어할 경우 이미지를 바꾸는 조건문을 추가하면 좋을듯.
			game_start_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			game_normal->enter();
			return 0;
			});
			*/
		return 0;
		});

		startGame(main);

}
