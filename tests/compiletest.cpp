#include <Texas/Texas.hpp>

struct TestStream : Texas::InputStream
{
public:
	[[nodiscard]] virtual Texas::Result read(Texas::ByteSpan dst) noexcept
	{
		return {};
	}
	virtual void ignore(std::size_t amount) noexcept
	{

	}

	[[nodiscard]] virtual std::size_t tell() noexcept
	{
		return 0;
	}

	virtual void seek(std::size_t pos) noexcept
	{

	}
};

int main()
{
	TestStream test{};
	Texas::ResultValue<Texas::Texture> texTexture = Texas::loadFromStream(test);

	return 0;
}