#ifndef File_h_
#define File_h_

#include <vector>

namespace litter {
	class File {
	public:
		static std::vector<char> readFile(const std::string& filename);
	};
}

#endif // !File_h_
