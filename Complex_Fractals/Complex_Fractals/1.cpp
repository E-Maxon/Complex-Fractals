#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class Complex {
	private:
		double a, b;
	public:
		Complex(double a, double b) : a(a), b(b) {}
		Complex operator*(Complex other) {
			return Complex(a * other.a - b * other.b, a * other.b + b * other.a);
		}
		Complex operator+(Complex other) {
			return Complex(a + other.a, b + other.b);
		}
		void add_re(double delta) {
			a += delta;
		}
		void add_im(double delta) {
			b += delta;
		}
		pair<double, double> get_koords() {
			return make_pair(a, b);
		}
		double abs() {
			return sqrt(a * a + b * b);
		}
};

int cnt(Complex c, Complex z) {
	int ans = 0;
	while (ans < 256 && z.abs() <= 2) {
		z = z * z + c;
		ans++;
	}
	return ans;/*256 - ans;*/
}

const int W = 1920;
const double w = W;
const int H = 1080;
const double h = H;
double lw = -2;
double rw = 2;
double lh = lw / w * h;
double rh = rw / w * h;

Complex to_complex(int x, int y) {
	//y = H - y;
	return Complex(lw + (rw - lw) * x / w, lh + (rh - lh) * y / h);
}

bool click;
Vector2f mouse_coords;
RectangleShape frame;

void zero_frame() {
	click = false;
	frame.setSize(Vector2f(0, 0));
	return;
}

sf::RenderWindow window(sf::VideoMode(1920, 1080), "Fractals", Style::Fullscreen);

Sprite background;

void recount(VertexArray& data, Complex c) {
	int ind = 0;
	window.draw(background);
	for (int x = 0; x < W; x++) {
		for (int y = 0; y < H; y++) {
			data[ind++].color = Color(0, 0, (cnt(c, to_complex(x, y)) - 120) * 2);
			//data[ind++].color = Color(0, 0, 0, cnt(c, to_complex(x, y)));
		}
		window.draw(data);
		window.display();
	}
}

struct node {
	VertexArray data;
	int lw, rw;
	int lh, rh;
	node(VertexArray data, int lw, int rw, int lh, int rh) : data(data), lw(lw), rw(rw), lh(lh), rh(rh) {}
};

int main() {
	Texture texture;
	texture.loadFromFile("zvezdy-kosmos.jpg");
	background.setTexture(texture);
	const Texture* for_frame = new Texture(texture);

    Complex c = Complex(-0.2, 0.8);
	const double delta = 0.5;

	//vector<vector<RectangleShape> > data(W, vector<RectangleShape>(H));
	vector<node> history;
	VertexArray data;
	for (int x = 0; x < W; x++) {
		for (int y = 0; y < H; y++) {
			Vertex tmp(Vector2f(x, y), Color(0, 0, (cnt(c, to_complex(x, y)) - 120) * 2));
			//Vertex tmp(Vector2f(x, y), Color(0, 0, 0, cnt(c, to_complex(x, y))));
			data.append(tmp);
		}
	}
	history.push_back(node(data, lw, rw, lh, rh));

	zero_frame();
	while (window.isOpen())
	{
		window.clear();

		Event key;
		while (window.pollEvent(key)) {
			if (Keyboard::isKeyPressed(Keyboard::Escape)) {
				window.close();
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				c.add_re(delta);
				recount(data, c);
			}
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				c.add_re(-delta);
				recount(data, c);
			}
			if (Keyboard::isKeyPressed(Keyboard::Up)) {
				c.add_im(delta);
				recount(data, c);
			}
			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				c.add_im(-delta);
				recount(data, c);
			}
			if ((key.type == Event::MouseButtonPressed && key.mouseButton.button == Mouse::Left) || click) {
				if (!click) {
					click = true;
					mouse_coords = window.mapPixelToCoords(Mouse::getPosition(window));
				}
				Vector2f mouse_coords2 = window.mapPixelToCoords(Mouse::getPosition(window));

				frame.setFillColor(Color(0, 0, 0, 0));
				frame.setOutlineColor(Color(255, 255, 255));
				frame.setOutlineThickness(2);
				frame.setTexture(for_frame);
				frame.setPosition(mouse_coords);

				auto tmp = mouse_coords2 - mouse_coords;
				tmp.y = tmp.x / W * H;

				frame.setSize(tmp);
			}
			if (key.type == Event::MouseButtonReleased) {
				auto tmp = frame.getPosition();
				auto tmp2 = frame.getSize();

				auto nlw = to_complex(tmp.x, tmp.y).get_koords().first;
				auto nrw = to_complex(tmp.x + tmp2.x, tmp.y + tmp2.y).get_koords().first;
				auto nlh = to_complex(tmp.x, tmp.y).get_koords().second;
				auto nrh = to_complex(tmp.x + tmp2.x, tmp.y + tmp2.y).get_koords().second;
				lw = nlw;
				rw = nrw;
				lh = nlh;
				rh = nrh;

				recount(data, c);
				zero_frame();
				history.push_back(node(data, lw, rw, lh, rh));
			}
			if (Keyboard::isKeyPressed(Keyboard::Backspace) && history.size() > 1) {
				history.pop_back();
				data = history.back().data;
				lw = history.back().lw;
				rw = history.back().rw;
				lh = history.back().lh;
				rh = history.back().rh;
			}
		}

		window.draw(background);
		window.draw(data);
		window.draw(frame);

		window.display();
	}

	return 0;
}