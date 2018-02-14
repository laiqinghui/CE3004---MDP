
sqwavesPerRev = 562.25
print "Enter squarewidth value(microSecond):"
squareWidth = float(raw_input())
print "Enter test run time(Second):"
runtime = float(raw_input())
sqwavesOneS = (1/(squareWidth*10**-6))
sqwavesPerRunTime = sqwavesOneS*runtime
sqwavesOneM = sqwavesOneS*60
revPerMin = sqwavesOneM/sqwavesPerRev
revPerRunTime = sqwavesPerRunTime/sqwavesPerRev

print "rev/min: %f " % (revPerMin)

print "rev/%f seconds: %f" % (runtime, revPerRunTime)
