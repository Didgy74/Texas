#include <Texas/Texas.hpp>

struct TestStream : Texas::InputStream
{
public:
	[[nodiscard]] virtual Texas::Result read(Texas::ByteSpan dst) noexcept override
	{
		return {};
	}
	virtual void ignore(std::size_t amount) noexcept override
	{

	}

	[[nodiscard]] virtual std::size_t tell() noexcept override
	{
		return 0;
	}

	virtual void seek(std::size_t pos) noexcept override
	{

	}
};

int main()
{
	TestStream test{};
	Texas::ResultValue<Texas::Texture> texTexture = Texas::loadFromStream(test);

	Texas::ResultValue<Texas::Texture> b = Texas::loadFromPath("Test.ktx2");

	return 0;
}