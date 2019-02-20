#include <Rrt2/Sampling.hpp>
#include <Rrt2/Vec3.hpp>
#include <pcg_variants.h>
#include <stb_image_write.h>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <ctime>

int main()
{
	const std::uint32_t kWidth = 64;
	const std::uint32_t kHeight = 64;
	const std::uint32_t kSizePerPixel = 3;
	pcg32_random_t randState;
	pcg32_srandom_r(&randState, std::time(0), (intptr_t)&randState);
	std::vector<std::byte> pngBytes(kWidth * kHeight * kSizePerPixel);
	Vec2 samples[8 * 8];
	Jitter(randState, gsl::make_span(samples));
	for (const Vec2& sample : samples)
	{
		const std::uint32_t destIndex = (static_cast<std::uint32_t>(sample.y * kHeight) * kWidth 
			+ static_cast<std::uint32_t>(sample.x * kWidth)) * kSizePerPixel;
		pngBytes[destIndex] = std::byte{ 255 };
		pngBytes[destIndex + 1] = std::byte{ 255 };
		pngBytes[destIndex + 2] = std::byte{ 255 };
	}

	//这里的 3 指的是 RGB。
	assert(stbi_write_png(".\\result.png", kWidth, kHeight, 3, pngBytes.data(), kWidth * kSizePerPixel) != 0);
}