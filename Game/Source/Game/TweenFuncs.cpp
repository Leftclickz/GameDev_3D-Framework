#include "GamePCH.h"

double BounceEaseIn(double t, double b, double c, double d);
double BounceEaseOut(double t, double b, double c, double d);
double BounceEaseInOut(double t, double b, double c, double d);
double ElasticEaseIn(double t, double b, double c, double d, float elasticity);
double ElasticEaseOut(double t, double b, double c, double d, float elasticity);
double ElasticEaseInOut(double t, double b, double c, double d, float elasticity);

float TweenFunc_Linear(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    float timeperc = (float)(timeElapsed/totalTime);
    if( timeperc > 1 )
        timeperc = 1;
    return startValue + valueRange*timeperc;
}

float TweenFunc_SineEaseIn(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    float timeperc = (float)(timeElapsed/totalTime);
    if( timeperc > 1 )
        timeperc = 1;
    return startValue + -valueRange * cos(timeperc * PI/2) + valueRange;
}

float TweenFunc_SineEaseOut(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    float timeperc = (float)(timeElapsed/totalTime);
    if( timeperc > 1 )
        timeperc = 1;
    return startValue + valueRange * sin(timeperc * PI/2);
}

float TweenFunc_SineEaseInOut(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    float timeperc = (float)(timeElapsed/totalTime);
    if( timeperc > 1 )
        timeperc = 1;
    return startValue + -valueRange/2 * (cos(timeperc * PI)-1);
}

float TweenFunc_BounceEaseIn(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    return (float)BounceEaseIn( timeElapsed, startValue, valueRange, totalTime );
}

float TweenFunc_BounceEaseOut(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    return (float)BounceEaseOut( timeElapsed, startValue, valueRange, totalTime );
}

float TweenFunc_BounceEaseInOut(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    return (float)BounceEaseInOut( timeElapsed, startValue, valueRange, totalTime );
}

float TweenFunc_ElasticEaseIn(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    return (float)ElasticEaseIn( timeElapsed, startValue, valueRange, totalTime, 3 );
}

float TweenFunc_ElasticEaseOut(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    return (float)ElasticEaseOut( timeElapsed, startValue, valueRange, totalTime, 3 );
}

float TweenFunc_ElasticEaseInOut(float startValue, float valueRange, double timeElapsed, double totalTime)
{
    return (float)ElasticEaseInOut( timeElapsed, startValue, valueRange, totalTime, 3 );
}

//http://www.kirupa.com/forum/archive/index.php/t-76799.html
double BounceEaseIn(double t, double b, double c, double d)
{
    return c - BounceEaseOut( d-t, 0, c, d ) + b;
}

double BounceEaseOut(double t, double b, double c, double d)
{
    if( (t/=d) < (1/2.75f) )
    {
        return c*(7.5625f*t*t) + b;
    }
    else if (t < (2/2.75f))
    {
        double postFix = t-=(1.5f/2.75f);
        return c*(7.5625f*(postFix)*t + .75f) + b;
    }
    else if (t < (2.5/2.75))
    {
        double postFix = t-=(2.25f/2.75f);
        return c*(7.5625f*(postFix)*t + .9375f) + b;
    }
    else
    {
        double postFix = t-=(2.625f/2.75f);
        return c*(7.5625f*(postFix)*t + .984375f) + b;
    }
}

double BounceEaseInOut(double t, double b, double c, double d)
{
    if( t < d/2 )
        return BounceEaseIn( t*2, 0, c, d ) * .5f + b;
    else
        return BounceEaseOut( t*2-d, 0, c, d ) * .5f + c*.5f + b;
}

double ElasticEaseIn(double t, double b, double c, double d, float elasticity)
{
    if( t == 0 )
        return b;
    if( t > d )
        return b+c;
    if( ( t /= d ) == 1 )
        return b+c;

    double p = d * 0.3f;
    double a = c;
    double s = p/4;

    double postFix = a * pow( 10, elasticity*(t-=1) );

    return -( postFix * sin( ( t*d-s ) * ( 2*PI )/p ) ) + b;
}

double ElasticEaseOut(double t, double b, double c, double d, float elasticity)
{
    if( t==0 )
        return b;
    if( t > d )
        return b+c;
    if( ( t /= d ) == 1 )
        return b+c;

    double p = d * 0.3f;
    double a = c;
    double s = p/4;

    return ( a * pow( 10, -elasticity*t ) * sin( ( t*d-s ) * ( 2*PI )/p ) + c + b);
}

double ElasticEaseInOut(double t, double b, double c, double d, float elasticity)
{
    if( t == 0 )
        return b;
    if( t > d )
        return b+c;
    if( ( t /= d/2 ) == 2 )
        return b+c;

    double p = d * ( 0.3f * 1.5f );
    double a = c;
    double s = p/4;

    if( t < 1 )
    {
        double postFix = a * pow( 10, elasticity*( t-=1 ) );
        return -0.5f * ( postFix * sin( ( t*d-s ) * ( 2*PI )/p ) ) + b;
    }

    double postFix = a * pow( 10, -elasticity*( t-=1 ) );
    return postFix * sin( ( t*d-s ) * ( 2*PI )/p ) * 0.5f + c + b;
}
