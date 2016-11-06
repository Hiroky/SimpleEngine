/**
 * ref. Precomputed Atmosphere Rendering / Eric Bruneton
 */

// Rayleigh scattering	
static const float HR = 8.0;
static const float3 betaRayleighScattering = float3(5.8e-3, 1.35e-2, 3.31e-2);

// Mie scattering
static const float HM = 1.2;
static const float3 betaMieScattering = 4e-3;
static const float3 betaMieScatteringExtinction = betaMieScattering / 0.9;
static const float mieG = 0.8;

static const float Rg = 6360.0;	// radius ground
static const float Rt = 6420.0;	// radius atmosphere
static const float RL = 6421.0; // redius limit

static const int TRANSMITTANCE_INTEGRAL_SAMPLES = 500;

#define TRANSMITTANCE_NON_LINEAR
#define INSCATTER_NON_LINEAR

//--------------------------------------------------------------------------------------

// nearest intersection of ray r,mu with ground or top atmosphere boundary
// mu=cos(ray zenith angle at ray origin)
float Limit(float r, float mu)
{
    float dout = -r * mu + sqrt(r * r * (mu * mu - 1.0) + RL * RL);
    float delta2 = r * r * (mu * mu - 1.0) + Rg * Rg;
    if (delta2 >= 0.0) {
        float din = -r * mu - sqrt(delta2);
        if (din >= 0.0) {
            dout = min(dout, din);
        }
    }
    return dout;
}

void GetTransmittanceRMu(float2 uv, out float r, out float muS) 
{
	r = uv.y;
	muS = uv.x;
#ifdef TRANSMITTANCE_NON_LINEAR
	r = Rg + (r * r) * (Rt - Rg);
	muS = -0.15 + tan(1.5 * muS) / tan(1.5) * (1.0 + 0.15);
#else
	r = Rg + r * (Rt - Rg);
	muS = -0.15 + muS * (1.0 + 0.15);
#endif
}

float OpticalDepth(float H, float r, float mu) 
{
    float result = 0.0;
    float dx = Limit(r, mu) / float(TRANSMITTANCE_INTEGRAL_SAMPLES);
    float xi = 0.0;
    float yi = exp(-(r - Rg) / H);
    for (int i = 1; i <= TRANSMITTANCE_INTEGRAL_SAMPLES; ++i) {
        float xj = float(i) * dx;
        float yj = exp(-(sqrt(r * r + xj * xj + 2.0 * xj * r * mu) - Rg) / H);
        result += (yi + yj) / 2.0 * dx;
        xi = xj;
        yi = yj;
    }
    return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? 1e9 : result;
}

//--------------------------------------------------------------------------------------
RWTexture2D<float> uavTransmittanceLUT : register( u0 );
[numthreads(16, 16, 1)]
void ComputeTransmittanceLUT(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint2 size;
	uavTransmittanceLUT.GetDimensions(size.x, size.y);
	float2 uv = (float2)dispatchThreadID.xy / size.xy;
	float r, muS;
	GetTransmittanceRMu(uv, r, muS);
    float3 depth = betaRayleighScattering * OpticalDepth(HR, r, muS) + betaMieScatteringExtinction * OpticalDepth(HM, r, muS);
	uavTransmittanceLUT[dispatchThreadID.xy] = float4(exp(-depth), 1.0); // Eq (5)
}
