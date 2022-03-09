#include "Particle.h"

#include <xmmintrin.h>
#include <immintrin.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include <ctime>

//rng, step 4 LCGs with intrinsics
//iq: https://www.iquilezles.org/~iquilezl/www/articles/sfrand/sfrand.htm
//_mm_or_si128
//_mm_srli_si128

Particle::Particle(size_t count, TileManager& tileManager)
: tileManager(tileManager)
{
	this->count = count;
	addParticles(glm::vec2(0), count, 58, glm::vec3(1));
    
	memset(particles, 0, sizeof(ParticleData) * 4096);
    
	union {
		alignas(16) int32_t s[4];
		__m128i ms;
	};
    
	union {
		alignas(16) uint32_t o[4];
		__m128i mo;
	};
    
	alignas(16) float f[4];
    
	s[0] = 12334;
	s[1] = 5435;
	s[2] = 6546;
	s[3] = 1324;
	mSeedsXVel = _mm_load_si128(&ms);
    
	s[0] = -4543;
	s[1] = -987;
	s[2] = -67;
	s[3] = -898;
	mSeedsYVel = _mm_load_si128(&ms);
    
	s[0] = 16807;
	s[1] = 16807;
	s[2] = 16807;
	s[3] = 16807;
	mFactor = _mm_load_si128(&ms);
    
	o[0] = 0x40000000;
	o[1] = 0x40000000;
	o[2] = 0x40000000;
	o[3] = 0x40000000;
	mOr = _mm_load_si128(&mo);
    
	o[0] = 0x3f800000;
	o[1] = 0x3f800000;
	o[2] = 0x3f800000;
	o[3] = 0x3f800000;
	mOr1 = _mm_load_si128(&mo);
    
	f[0] = -17.0f;
	f[1] = f[0];
	f[2] = f[0];
	f[3] = f[0];
	m17 = _mm_load_ps(f);
    
	f[0] = 3.0f;
	f[1] = 3.0f;
	f[2] = 3.0f;
	f[3] = 3.0f;
	mThree = _mm_load_ps(f);
    
	f[0] = 1.0f;
	f[1] = 1.0f;
	f[2] = 1.0f;
	f[3] = 1.0f;
	mOne = _mm_load_ps(f);
    
    
}

float Particle::sfrand(int* seed)
{
	float res;
	seed[0] *= 16807;
	*((unsigned int*)&res) = (((unsigned int)seed[0]) >> 9) | 0x40000000;
	return(res - 3.0f);
}

__m128 Particle::simd_sfrand(__m128i& seeds)
{
	//seed[0] *= 16807;
	seeds = muly(seeds, mFactor);
	
	//*((unsigned int*)&res) = (((unsigned int)seed[0]) >> 9) | 0x40000000;
	__m128i r = _mm_or_si128(_mm_srli_epi32(seeds, 9), mOr);	
	__m128 rFloat = _mm_castsi128_ps(r); //"Cast" (not really) to float
    
	//return(res - 3.0f);
	return _mm_sub_ps(rFloat, mThree);
}

__m128 Particle::simd_frand(__m128i& seeds)
{
	//seed[0] *= 16807;
	seeds = muly(seeds, mFactor);
    
	//*((unsigned int*)&res) = (((unsigned int)seed[0]) >> 9) | 0x3f800000;
	__m128i r = _mm_or_si128(_mm_srli_epi32(seeds, 9), mOr1);
	__m128 rFloat = _mm_castsi128_ps(r); //"Cast" (not really) to float
    
	//return(res - 1.0f);
	return _mm_sub_ps(rFloat, mOne);
}

void Particle::updateSprings()
{
	float zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	__m128 mZero = _mm_load_ps(zero);
    
	for (int x = 0; x < springCount; x++)
	{
		SpringData& spring = springs[x];
		int aInd = spring.indexA;
		int bInd = spring.indexB;
		ParticleData* a = spring.ptA;
		ParticleData* b = spring.ptB;
        
		a->xForce[aInd] = 0.0f;
		a->yForce[aInd] = spring.pinnedA ? 0 : spring.gravityA;
		b->xForce[bInd] = 0.0f;
		b->yForce[bInd] = spring.pinnedB ? 0 : spring.gravityB;
	}
    
	for (int x = 0; x < springCount; x++)
	{
		SpringData& spring = springs[x];
		int aInd = spring.indexA;
		int bInd = spring.indexB;
		ParticleData* a = spring.ptA;
		ParticleData* b = spring.ptB;
        
		glm::vec2 diff(b->x[bInd] - a->x[aInd], b->y[bInd] - a->y[aInd]);
		//if (diff.x < 1.0f) diff.x = 0.0f;
		//if (diff.y < 1.0f) diff.y = 0.0f;
		float len = glm::length(diff);
        
		float comp = len - spring.len;
		glm::vec2 vel(b->xVel[bInd] - a->xVel[aInd], b->yVel[bInd] - a->yVel[aInd]);
        
		float force = (comp * spring.spring) + (spring.damp * glm::dot(diff / len, vel));
		glm::vec2 forceVec = force * diff;
        
		//if (abs(forceVec.x) < 0.1f) forceVec.x = 0.0f;
        
		a->xForce[aInd] += spring.pinnedA > 0.5f ? 0 : forceVec.x;
		a->yForce[aInd] += spring.pinnedA > 0.5f ? 0 : forceVec.y;
		b->xForce[bInd] += spring.pinnedB > 0.5f ? 0 : -forceVec.x;
		b->yForce[bInd] += spring.pinnedB > 0.5f ? 0 : -forceVec.y;
	}
}

void Particle::updateParticles(float deltaTime)
{
	updateSprings();
    
	__m128 dt = _mm_load1_ps(&deltaTime);
	size_t added = 0;
	for (int i = 0; i < count/4; i++)
	{
		//std::cout << "a\n";
		ParticleData& p = particles[i];
        
		__m128 mTime = _mm_load_ps(p.timeAlive);
		if (p.timeAlive[0] >= 0.0f)
		{
			mTime = _mm_add_ps(mTime, dt);
			_mm_store_ps(p.timeAlive, mTime);
		}
        
		if (p.timeAlive[0] > 0.5f)
		{
			__m128 mZero = _mm_load_ps(zero);
			_mm_store_ps(p.timeAlive, mZero);
			//_mm_store_ps(p.x, mZero);
			//_mm_store_ps(p.y, mZero);
            
			//_mm_store_ps(p.xVel, _mm_mul_ps(simd_sfrand(mSeedsXVel), m17));
			//_mm_store_ps(p.yVel, _mm_mul_ps(simd_sfrand(mSeedsYVel), m17));
            
            //			_mm_store_ps(&glows[i * 4], simd_frand(mSeedsXVel));
            
			_mm_store_ps(p.alive, mZero);
			//we can overwrite these particles when new ones are added
			continue;
		}
		else if (p.alive[0] < 0.5f)
		{
			continue;
		}
        
		_mm_store_ps(&times[i * 4], mTime);
		//std::cout << "asdf x " << p.x[0] << " y " << p.y[0] << "\n";
        
		__m128 mX = _mm_load_ps(p.x);
		__m128 mY = _mm_load_ps(p.y);
        
		__m128 mVelX = _mm_load_ps(p.xVel);
		__m128 mVelY = _mm_load_ps(p.yVel);
        
		__m128 mAccX = _mm_load_ps(p.xAcc);
		__m128 mAccY = _mm_load_ps(p.yAcc);
        
		__m128 mForX = _mm_load_ps(p.xForce);
		__m128 mForY = _mm_load_ps(p.yForce);
        
		__m128 mMass = _mm_load_ps(p.mass);
        
		mAccX = _mm_div_ps(mForX, mMass);
		mAccY = _mm_div_ps(mForY, mMass);
        
		mVelX = _mm_add_ps(mVelX, _mm_mul_ps(mAccX, dt));
		mVelY = _mm_add_ps(mVelY, _mm_mul_ps(mAccY, dt));
        
		mX = _mm_add_ps(mX, _mm_mul_ps(mVelX, dt));
		mY = _mm_add_ps(mY, _mm_mul_ps(mVelY, dt));
        
		_mm_store_ps(p.x, mX);
		_mm_store_ps(p.y, mY);
        
		_mm_store_ps(p.xVel, mVelX);
		_mm_store_ps(p.yVel, mVelY);
        
		_mm_store_ps(p.xAcc, mAccX);
		_mm_store_ps(p.yAcc, mAccY);
        
		//change mesh class to only look at first "count" in the positions array
		float* pos = &posi[i * 4].x;
		__m128 xyLow = _mm_unpacklo_ps(mX, mY);
		__m128 xyHigh = _mm_unpackhi_ps(mX, mY);
        
		_mm_store_ps(pos, xyLow);
		_mm_store_ps(pos + 4, xyHigh);
	}
    
	/* Temp spring collision */
	ParticleData& p = particles[0];
	for (int i = 0; i < 4; i++)
	{
		glm::vec2 p1(p.x[i], p.y[i]);
		for (int j = 0; j < 4; j++)
		{
			if (i == j) continue;
			glm::vec2 p2(p.x[j], p.y[j]);
			glm::vec2 dir = p2 - p1;
			float len = glm::length(dir);
			if (len < 3.0f)
			{
				glm::vec2 dirNorm = glm::normalize(dir);
				float mag = 3.0f - glm::length(dir);
                
				dirNorm *= mag;
                
				p.x[j] += dirNorm.x;
				p.y[j] += dirNorm.y;
			}
		}
	}
    
	ParticleData temp[4096];
	glm::vec2 tempPos[4096];
	glm::vec4 tempUv[4096];
	float tempSize[4096];
	glm::vec3 tempColor[4096];
	
	size_t ct = 0;
	for (int i = 0; i < count / 4; i++)
	{
		ParticleData& p = particles[i];
		if (p.alive[0] < 0.5f)
		{
			continue;
		}
		else
		{
			memcpy(temp + ct, particles + i, sizeof(ParticleData));
			memcpy(tempPos + (ct * 4), posi + (i * 4), sizeof(glm::vec2) * 4);
			memcpy(tempUv + (ct * 4), uv + (i * 4), sizeof(glm::vec4) * 4);
			memcpy(tempSize + (ct * 4), sizes + (i * 4), sizeof(float) * 4);
			memcpy(tempColor + (ct * 4), colors + (i * 4), sizeof(glm::vec3) * 4);
			ct++;
		}
	}
    
	memcpy(particles, temp, sizeof(ParticleData) * ct);
	memcpy(posi, tempPos, sizeof(glm::vec2) * (ct * 4));
	memcpy(uv, tempUv, sizeof(glm::vec4) * (ct * 4));
	memcpy(sizes, tempSize, sizeof(float) * (ct * 4));
	memcpy(colors, tempColor, sizeof(glm::vec3) * (ct * 4));
    
	this->count = ct*4;
}

void Particle::addParticles(glm::vec2 pos, size_t amount, unsigned int id, glm::vec3 c)
{
	float px[4] = { pos.x, pos.x, pos.x, pos.x };
	float py[4] = { pos.y, pos.y, pos.y, pos.y };
	float five[4] = { 8.0f, 8.0f, 8.0f, 8.0f };
	float xOff[4] = { 5.0f, 5.0f, 5.0f, 5.0f };
    float size[4] = { 2.0f, 2.0f, 2.0f, 2.0f };
    float zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	__m128 mPosY = _mm_load_ps(py);
	__m128 mFive = _mm_load_ps(five);
	__m128 mZero = _mm_load_ps(zero);
    __m128 mSize = _mm_load_ps(size);
    __m128 mXOff = _mm_load_ps(xOff);
    
    __m128 mPosX = _mm_add_ps(_mm_load_ps(px), _mm_mul_ps(simd_sfrand(mSeedsXVel), mXOff));
    
	glm::vec4 uv = glm::vec4(0, 0, 1, 1);
	__m128 mUv = _mm_load_ps(reinterpret_cast<float*>(&uv));
    
	glm::vec3 cs[4] = { c, c, c, c};
	float* fcs = reinterpret_cast<float*>(cs);
	__m128 mC = _mm_load_ps(fcs);
	__m128 mC1 = _mm_load_ps(fcs + 4);
	__m128 mC2 = _mm_load_ps(fcs + 8);
    
	for (int x = 0; x < amount/4; x++)
	{
		ParticleData p{};
		_mm_store_ps(p.x, mPosX);
		_mm_store_ps(p.y, mPosY);
		_mm_store_ps(p.xVel, _mm_mul_ps(simd_sfrand(mSeedsXVel), mFive));
		_mm_store_ps(p.yVel, _mm_mul_ps(simd_frand(mSeedsYVel), m17));
		_mm_store_ps(p.xAcc, mZero);
		_mm_store_ps(p.yAcc, mZero);
		_mm_store_ps(p.xForce, mZero);
		_mm_store_ps(p.yForce, m17);
		_mm_store_ps(p.mass, mOne);
		_mm_store_ps(p.alive, mOne);
        
		particles[count/4 + x] = p;
        
		float* pf = &posi[count + (x * 4)].x;
        
		__m128 xyLow = _mm_unpacklo_ps(mPosX, mPosY);
		__m128 xyHigh = _mm_unpackhi_ps(mPosX, mPosY);
		_mm_store_ps(pf, xyLow);
		_mm_store_ps(pf + 4, xyHigh);
        
		float* puv = &this->uv[count + (x * 4)].x;
		_mm_store_ps(puv, mUv);
		_mm_store_ps(puv + 4, mUv);
		_mm_store_ps(puv + 8, mUv);
		_mm_store_ps(puv + 12, mUv);
        
		float* psize = &sizes[count + (x * 4)];
		_mm_store_ps(psize, mSize);
        
		float* ptime = &times[count + (x * 4)];
		_mm_store_ps(ptime, mZero);
        
		float* pcolor = &colors[count + (x * 4)].x;
		_mm_store_ps(pcolor, mC);
		_mm_store_ps(pcolor + 4, mC1);
		_mm_store_ps(pcolor + 8, mC2);
	}
    
	count += amount;
}

void Particle::addBoxSpring(glm::vec2 pos, int size)
{
	float zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float neg[4] = { -1.0f, -1.0f, -1.0f, -1.0f };
	__m128 mZero = _mm_load_ps(zero);
	__m128 mNeg = _mm_load_ps(neg);
    
	addParticles(pos, 4, 59, glm::vec3(0));
	SpringData spring{};
	SpringData spring1{};
	SpringData spring2{};
	SpringData spring3{};
	SpringData spring4{};
    
	ParticleData& part = particles[count/4 - 1];
	_mm_store_ps(part.timeAlive, mNeg);
    
	float s = size;
	part.x[2] += s;
	part.x[3] += s;
    
	part.y[1] += s + 100.0f;
	part.y[3] += s + 100.0f;
    
	float* psize = &sizes[count - 4];
	_mm_store_ps(psize, mOne);
    
	_mm_store_ps(part.xVel, mZero);
	_mm_store_ps(part.yVel, mZero);
	_mm_store_ps(part.yForce, mZero);
    
	float spCt = 2.0f;
	float spDamp = 0.1f;
    
	//spring 0
	spring.ptA = &particles[count/4 - 1];
	spring.ptB = &particles[count / 4 - 1];
    
	spring.indexA = 0;
	spring.indexB = 1;
    
	spring.gravityA = 0.0f;
	spring.gravityB = 17.0f;
    
	spring.len = s;
	spring.spring = spCt;
	spring.damp = spDamp;
    
	spring.pinnedA = 1.0f;
	spring.pinnedB = 0.0f;
    
	//spring 1;
	spring1.ptA = &particles[count / 4 - 1];
	spring1.ptB = &particles[count / 4 - 1];
    
	spring1.indexA = 2;
	spring1.indexB = 3;
    
	spring1.gravityA = 0.0f;
	spring1.gravityB = 17.0f;
    
	spring1.len = s;
	spring1.spring = spCt;
	spring1.damp = spDamp;
    
	spring1.pinnedA = 1.0f;
	spring1.pinnedB = 0.0f;
    
	//spring 2;
	spring2.ptA = &particles[count / 4 - 1];
	spring2.ptB = &particles[count / 4 - 1];
    
	spring2.indexA = 1;
	spring2.indexB = 3;
    
	spring2.gravityA = 17.0f;
	spring2.gravityB = 17.0f;
    
	spring2.len = s;
	spring2.spring = 20.0f;
	spring2.damp = 0.1f;
    
	spring2.pinnedA = 0.0f;
	spring2.pinnedB = 0.0f;
    
	//spring 3;
	spring3.ptA = &particles[count / 4 - 1];
	spring3.ptB = &particles[count / 4 - 1];
    
	spring3.indexA = 1;
	spring3.indexB = 2;
    
	spring3.gravityA = 17.0f;
	spring3.gravityB = 0.0f;
    
	spring3.len = sqrt(s*s + s*s);
	spring3.spring = spCt;
	spring3.damp = spDamp;
    
	spring3.pinnedA = 0.0f;
	spring3.pinnedB = 1.0f;
    
	//spring 4;
	spring4.ptA = &particles[count / 4 - 1];
	spring4.ptB = &particles[count / 4 - 1];
    
	spring4.indexA = 0;
	spring4.indexB = 3;
    
	spring4.gravityA = 0.0f;
	spring4.gravityB = 17.0f;
    
	spring4.len = sqrt(s * s + s * s);
	spring4.spring = spCt;
	spring4.damp = spDamp;
    
	spring4.pinnedA = 1.0f;
	spring4.pinnedB = 0.0f;
    
	springs[springCount] = spring;
	springs[springCount + 1] = spring1;
	springs[springCount + 2] = spring2;
	springs[springCount + 3] = spring3;
	springs[springCount + 4] = spring4;
    
	springCount += 5;
}