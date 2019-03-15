#include "Camera.hpp"
#include "Sampling.hpp"
#include <DirectXMath.h>
#include "DxMathHelpers.hpp"
#include "Ray.hpp"

using namespace DirectX;

class alignas(16) CameraData
{
  public:
    XMVECTOR uAxis, vAxis, wAxis;
    XMVECTOR filmLeftBottomCorner;
    XMVECTOR center;
};

Camera::Camera(std::uint32_t filmWidth, std::uint32_t filmHeight, float radius, float distance,
               const Vec3f& center)
    : m_filmWidth{filmWidth}, m_filmHeight{filmHeight}, m_radius{radius},
      m_distance{distance}, m_data{std::make_unique<CameraData>()}
{

    m_data->center = LoadFloat3(&center.x);
}

void Camera::SetLookAt(const Vec3f& eye, const Vec3f& focus, const Vec3f& up)
{
    const XMVECTOR eyeVec = LoadFloat3(eye);
    const XMVECTOR focusVec = LoadFloat3(focus);
    const XMVECTOR upVec = LoadFloat3(up);
    m_data->wAxis = XMVector3Normalize(focusVec - eyeVec);
    XMVECTOR u = XMVector3Cross(upVec, m_data->wAxis);
    m_data->uAxis = XMVector3Normalize(u);
    m_data->vAxis = XMVector3Cross(m_data->wAxis, u);
    m_data->filmLeftBottomCorner = m_data->center - XMVectorSet(0.5f, 0.5f, m_distance, 0.0f);
    // m_data->
}

void Camera::GetRay(const Vec2f& pixelPos, const Vec2f& thinLensPos, Ray& ray)
{
    const XMVECTOR thinLens = LoadFloat2(&thinLensPos.x);
    ray.origin = thinLens * m_data->uAxis + thinLens * m_data->vAxis + m_data->center;
    const XMVECTOR target =
        m_data->filmLeftBottomCorner + pixelPos.x * m_data->uAxis + pixelPos.y * m_data->vAxis;
    ray.speed = XMVector3Normalize(target - ray.origin);
}

void Camera::GenerateRays(pcg32_random_t& state, AlignedVec<Ray>& rays,
                          std::uint32_t samplesPerPixel)
{
    const std::uint32_t filmWidth = m_filmWidth;
    const std::uint32_t filmHeight = m_filmHeight;
    const std::uint32_t samplesCount = filmWidth * filmHeight * samplesPerPixel;
    rays.reserve(samplesCount);
    std::vector<Vec2f> thinLensPoses(samplesPerPixel);
    std::vector<Vec2f> pixelPoses(samplesPerPixel);
    for (std::uint32_t w = 0; w < filmWidth; ++w)
    {
        // float wStart = w / static_cast<float>(filmWidth);
        for (std::uint32_t h = 0; h < filmHeight; ++h)
        {
            // float hStart = h / static_cast<float>(filmHeight);
            Jitter(state, gsl::make_span(thinLensPoses));
            Jitter(state, gsl::make_span(pixelPoses));
            // BoxFilter(gsl::make_span(pixelPoses));
            for (std::uint32_t i = 0; i < samplesPerPixel; ++i)
            {
                Ray ray;
                Vec2f& pixelPos = pixelPoses[i];
                pixelPos.x = (pixelPos.x - 0.5f + w) / static_cast<float>(filmWidth);
                pixelPos.y = (pixelPos.y - 0.5f + h) / static_cast<float>(filmHeight);
                Vec2f& thinLensPos = thinLensPoses[i];
                thinLensPos.x -= 0.5f;
                thinLensPos.y -= 0.5f;
                GetRay(pixelPos, thinLensPos, ray);
                rays.push_back(ray);
            }
        }
    }
}

// void Camera::GetRay(const Vec2fPacked& pixelPos, const Vec2fPacked& thinLensPos, SimdRay& ray)
//{
//	const PackedFloats half = MakeFloats(0.5f);
//	const PackedFloats thinLensX = Mul(Sub(thinLensPos.x, half), m_data->uAxis);
//	const PackedFloats thinLensY = Mul(Sub(thinLensPos.y, half), m_data->vAxis);
//	const PackedFloats pixelX = Mul(m_data->across,
//}
