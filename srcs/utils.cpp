#include "../includes/Utils.hpp"

void	printUsage(void)
{
    std::cout << "Usage: ./snake [OPTIONS]\n\n";
    std::cout << "Description:\n";
    std::cout << "  A Reinforcement Learning Snake agent using Q-Learning.\n\n";

    std::cout << "Options:\n";
    std::cout << "  --session <n>    Number of training sessions/episodes to run.\n";
    std::cout << "  --load <file>    Load a pre-trained Q-Table/Model from a JSON/txt file.\n";
    std::cout << "  --nolearn        Disable Q-Table updates (useful for testing a loaded model).\n";
    std::cout << "  --novisual       Train the model without the graphical interface (faster training).\n";
    std::cout << "  --help           Display this help message and exit.\n\n";

    std::cout << "Examples:\n";
    std::cout << "./snake --session 50000\n";
    std::cout << "./snake --load Session01.json --nolearn\n";
    std::cout << "./snake --novisual --session 100000\n\n";

    std::cout << "Lobby Controls (Visual Mode Only):\n";
    std::cout << "  - Use the slider to adjust game speed (FPS).\n";
    std::cout << "  - Click 'Export' to save the current Q-Table as SessionXX.json.\n";
    std::cout << "  - Toggle Auto/Step-by-step mode to debug the agent's decisions.\n";
}

bool parseArgs(int ac, char** av, Config& cfg)
{
	for (int i = 1; i < ac; ++i)
	{
		std::string arg = av[i];

		if (arg == "--help") return false;
		else if (arg == "--nolearn") cfg.learn = false;
		else if (arg == "--novisual") cfg.visual = false;
		else if (arg == "--session")
		{
			if (i + 1 < ac) cfg.sessions = std::stoi(av[++i]);
			else
			{
				std::cerr << "Error: --session requires a value.\n";
				return false;
			}
		}
		else if (arg == "--load")
		{
			if (i + 1 < ac) cfg.loadFile = av[++i];
			else
			{
				std::cerr << "Error: --load requires a file path.\n";
				return false;
			}
		}
		else
		{
			std::cerr << "Error: Unknown argument '" << arg << "'\n";
			return false;
		}
	}
	return true;
}

void	update(uint8_t s, uint8_t a, float r, uint8_t s2, std::vector<std::vector<float>>& qtable)
{
	float alpha = 0.1, gamma = 0.25;
	float maxNext = std::max({qtable[s2][0], qtable[s2][1], qtable[s2][2]});
	qtable[s][a] += alpha * (r + gamma * maxNext - qtable[s][a]);
}

void exportModel(const std::vector<std::vector<float>>& qtable, const Config& cfg, int sessionID)
{
	std::string filename = "Session" + std::to_string(sessionID) + ".json";
	std::ofstream file(filename);
	if (!file.is_open()) return;

	file << "{\n";
	file << "  \"metadata\": {\n";
	file << "    \"session\": " << sessionID << ",\n";
	file << "    \"grid width\": " << GRID_W << ",\n";
	file << "    \"grid height\": " << GRID_H << "\n";
	file << "  },\n";

	file << "  \"rewards\": {\n";
	file << "    \"green_apple\": " << cfg.rewardGreen << ",\n";
	file << "    \"red_apple\": " << cfg.rewardRed << ",\n";
	file << "    \"death\": " << cfg.rewardDie << ",\n";
	file << "    \"idle\": " << cfg.rewardIdle << ",\n";
	file << "    \"closer\": " << cfg.rewardCloser << "\n";
	file << "  },\n";

	file << "  \"qtable\": [\n";
	file << std::fixed << std::setprecision(4);
	for (size_t i = 0; i < qtable.size(); ++i) {
		file << "    [" << qtable[i][0] << ", " << qtable[i][1] << ", " << qtable[i][2] << "]";
		if (i < qtable.size() - 1) file << ",";
		file << "\n";
	}
	file << "  ]\n";
	file << "}";

	file.close();
}
