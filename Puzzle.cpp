#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

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

bool check_clear(struct puzzle* p, int* x_answer, int* y_answer)
{
	for (int r = 0; r < 12; r++)
	{
		if (p[r].x != x_answer[r] || p[r].y != y_answer[r])
			return false;
	}
	return true;
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
	int clear_time = 0; // 1초씩 감소시키면서 클리어까지 걸린 시간 체크
	char time_message[4] = { 0, };
	char time_finish[40] = "초 남기고 클리어하셨습니다!";

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

	int x_answer[12] = { -480, -480, -480, -160, -160, -160, 160, 160, 160, 480, 480, 480 };
	int y_answer[12] = { -240, 0, 240, -240, 0, 240, -240, 0, 240, -240, 0, 240 };

	auto timer = Timer::create(1200.f);
	timer->setOnTimerCallback([&](TimerPtr)->bool {
		showMessage("아쉽습니다. \n다시 시도하고 싶다면 오른쪽 아래 다시하기 버튼을 눌러보세요.");
		return 0;
		});

	auto game_start_button = Object::create("Images/게임시작버튼.png", select, 596, 44, false);
	auto game_preview = Object::create("Images/조커_선택창.jpg", select, 512, 288, false);
	int temp_x = 0;
	int temp_y = 0;
	int i = 0;

	auto restart = Object::create("Images/다시하기.png", game_easy, 650, -200, true);
	restart->setScale(0.2f);
	restart->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		i = 0;
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
		int blank_index = rand() % 12;
		timer->stop();
		timer->set(1200.f);
		hideTimer();
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

		p[0].puzzle_piece->setImage("Images/빈 조각.png");
		int blank_index = 0;

		p[1].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[1].x, p[1].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[1].x + x, p[1].y + y, p, blank_index) || rcheck(p[1].x + x, p[1].y + y, p, blank_index) || upcheck(p[1].x + x, p[1].y + y, p, blank_index) || downcheck(p[1].x + x, p[1].y + y, p, blank_index))
			{
				temp_x = p[1].x;
				temp_y = p[1].y;
				p[1].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[1].x = p[blank_index].x;
				p[1].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n"); 
					timer->stop();
				}
			}
			return 0;
			});

		p[2].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[2].x, p[2].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[2].x + x, p[2].y + y, p, blank_index) || rcheck(p[2].x + x, p[2].y + y, p, blank_index) || upcheck(p[2].x + x, p[2].y + y, p, blank_index) || downcheck(p[2].x + x, p[2].y + y, p, blank_index))
			{
				temp_x = p[2].x;
				temp_y = p[2].y;
				p[2].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[2].x = p[blank_index].x;
				p[2].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[3].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[3].x, p[3].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[3].x + x, p[3].y + y, p, blank_index) || rcheck(p[3].x + x, p[3].y + y, p, blank_index) || upcheck(p[3].x + x, p[3].y + y, p, blank_index) || downcheck(p[3].x + x, p[3].y + y, p, blank_index))
			{
				temp_x = p[3].x;
				temp_y = p[3].y;
				p[3].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[3].x = p[blank_index].x;
				p[3].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[4].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[4].x, p[4].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[4].x + x, p[4].y + y, p, blank_index) || rcheck(p[4].x + x, p[4].y + y, p, blank_index) || upcheck(p[4].x + x, p[4].y + y, p, blank_index) || downcheck(p[4].x + x, p[4].y + y, p, blank_index))
			{
				temp_x = p[4].x;
				temp_y = p[4].y;
				p[4].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[4].x = p[blank_index].x;
				p[4].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[5].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[5].x, p[5].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[5].x + x, p[5].y + y, p, blank_index) || rcheck(p[5].x + x, p[5].y + y, p, blank_index) || upcheck(p[5].x + x, p[5].y + y, p, blank_index) || downcheck(p[5].x + x, p[5].y + y, p, blank_index))
			{
				temp_x = p[5].x;
				temp_y = p[5].y;
				p[5].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[5].x = p[blank_index].x;
				p[5].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[6].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[6].x, p[6].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[6].x + x, p[6].y + y, p, blank_index) || rcheck(p[6].x + x, p[6].y + y, p, blank_index) || upcheck(p[6].x + x, p[6].y + y, p, blank_index) || downcheck(p[6].x + x, p[6].y + y, p, blank_index))
			{
				temp_x = p[6].x;
				temp_y = p[6].y;
				p[6].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[6].x = p[blank_index].x;
				p[6].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[7].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[7].x, p[7].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[7].x + x, p[7].y + y, p, blank_index) || rcheck(p[7].x + x, p[7].y + y, p, blank_index) || upcheck(p[7].x + x, p[7].y + y, p, blank_index) || downcheck(p[7].x + x, p[7].y + y, p, blank_index))
			{
				temp_x = p[7].x;
				temp_y = p[7].y;
				p[7].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[7].x = p[blank_index].x;
				p[7].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[8].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[8].x, p[8].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[8].x + x, p[8].y + y, p, blank_index) || rcheck(p[8].x + x, p[8].y + y, p, blank_index) || upcheck(p[8].x + x, p[8].y + y, p, blank_index) || downcheck(p[8].x + x, p[8].y + y, p, blank_index))
			{
				temp_x = p[8].x;
				temp_y = p[8].y;
				p[8].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[8].x = p[blank_index].x;
				p[8].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[9].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[9].x, p[9].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[9].x + x, p[9].y + y, p, blank_index) || rcheck(p[9].x + x, p[9].y + y, p, blank_index) || upcheck(p[9].x + x, p[9].y + y, p, blank_index) || downcheck(p[9].x + x, p[9].y + y, p, blank_index))
			{
				temp_x = p[9].x;
				temp_y = p[9].y;
				p[9].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[9].x = p[blank_index].x;
				p[9].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[10].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[10].x, p[10].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[10].x + x, p[10].y + y, p, blank_index) || rcheck(p[10].x + x, p[10].y + y, p, blank_index) || upcheck(p[10].x + x, p[10].y + y, p, blank_index) || downcheck(p[10].x + x, p[10].y + y, p, blank_index))
			{
				temp_x = p[10].x;
				temp_y = p[10].y;
				p[10].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[10].x = p[blank_index].x;
				p[10].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
			}
			return 0;
			});

		p[11].puzzle_piece->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			printf("%d %d\n", p[11].x, p[11].y);
			printf("%d %d\n", x, y);
			if (lcheck(p[11].x + x, p[11].y + y, p, blank_index) || rcheck(p[11].x + x, p[11].y + y, p, blank_index) || upcheck(p[11].x + x, p[11].y + y, p, blank_index) || downcheck(p[11].x + x, p[11].y + y, p, blank_index))
			{
				temp_x = p[11].x;
				temp_y = p[11].y;
				p[11].puzzle_piece->locate(game_easy, p[blank_index].x, p[blank_index].y);
				p[11].x = p[blank_index].x;
				p[11].y = p[blank_index].y;
				p[blank_index].puzzle_piece->locate(game_easy, temp_x, temp_y);
				p[blank_index].x = temp_x;
				p[blank_index].y = temp_y;
				if (check_clear(p, x_answer, y_answer)) // 남은 시간을 어떻게 출력할지 생각해보자.
				{
					showMessage("축하합니다!\n");
					timer->stop();
				}
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
