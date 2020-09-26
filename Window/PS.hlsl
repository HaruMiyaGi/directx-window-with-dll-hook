cbuffer CBuf
{
    float4 face_color[6];
};

float4 main(uint tid : SV_PRIMITIVEID) : SV_TARGET
{
    return face_color[(tid / 2) % 6];
}