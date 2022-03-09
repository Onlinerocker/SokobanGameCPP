#pragma once
#include "TileManager.h"

#include <vector>
#include <glm/glm.hpp>
#include <emmintrin.h>
#include <smmintrin.h>

struct ParticleData
{
	alignas(16) float timeAlive[4] = { 0 };

	alignas(16) float x[4] = { 0 };
	alignas(16) float y[4] = { 0 };

	alignas(16) float xVel[4] = { 0 };
	alignas(16) float yVel[4] = { 0 };

	alignas(16) float xAcc[4] = { 0 };
	alignas(16) float yAcc[4] = { 0 };

	alignas(16) float xForce[4] = { 0 };
	alignas(16) float yForce[4] = { 0 };

	alignas(16) float mass[4] = { 1.0 };

	alignas(16) float alive[4] = { 1.0 };
};

struct SpringData
{
	ParticleData* ptA;
	ParticleData* ptB;

	int indexA; //index of point A in data A
	int indexB; //index of point B in data B
	
	float gravityA = 0.0f;
	float gravityB = 0.0f;

	float len; //resting length
	float damp; //dampening constant
	float spring; //spring constant

	float pinnedA = 0.0f;
	float pinnedB = 0.0f;
};

class Particle
{
private:
	alignas(16) __m128i mSeedsXVel;
	alignas(16) __m128i mSeedsYVel;

	alignas(16) __m128i mFactor;
	alignas(16) __m128i mOr;
	alignas(16) __m128i mOr1;
	alignas(16) __m128 m17;
	alignas(16) __m128 mThree;
	alignas(16) __m128 mOne;

	TileManager& tileManager;

	float sfrand(int* seed);
	__m128 simd_sfrand(__m128i& seeds);
	__m128 simd_frand(__m128i& seeds);

	//from https://stackoverflow.com/questions/10500766/sse-multiplication-of-4-32-bit-integers
	static inline __m128i muly(const __m128i& a, const __m128i& b)
	{
		__m128i tmp1 = _mm_mul_epu32(a, b); /* mul 2,0*/
		__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4)); /* mul 3,1 */
		return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0, 0, 2, 0))); /* shuffle results to [63..0] and pack */
	};


public:
	Particle(size_t count, TileManager& tileManager);

	alignas(16) ParticleData particles[4096];
	alignas(16) float zero[4] = { 0 };

	alignas(16) glm::vec2 posi[16384];
	alignas(16) glm::vec4 uv[16384];
	alignas(16) float sizes[16384];
	alignas(16) float times[16384];
	alignas(16) glm::vec3 colors[16384];

	size_t count;

	SpringData springs[128];
	size_t springCount;

	ParticleData* curSpringPart;
	float* pinned;
	int curSpringPartInd;
	glm::vec2 curSpringOrigin;
	glm::vec2 mouseOrigin;

	void updateSprings();
	void updateParticles(float deltaTime);
	void addParticles(glm::vec2 pos, size_t amount, unsigned int id, glm::vec3 color);
	void addBoxSpring(glm::vec2 pos, int size);
};

//on mouse down: get closest particle, get position, "pin" particle
//if mouse down: set cur particle to: origin + (mouse origin - curMouse)
//on mouse up: "unpin" current particle