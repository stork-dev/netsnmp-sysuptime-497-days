# SNMP Issue: sysUpTime rolls over on 497th Day


Data type of sysUpTime in snmp is TimeTicks which is again a Counter32 in units of hundredth of a second. If we divide value by 100 then it is converted to seconds and increases rolling over limit to 100 times and you will avoid false alarms like device restarted But Again this will break RFC implementation, you have to customize/reconfigure manager who is polling sysUpTime.


Since TimeTicks is a Counter32 in units of hundredth of a second  So Max value of TimeTicks is 497 days approximately.
```
Counter32 max value is 4294967295.
Seconds in a day are 86400.
100 Counter32 = 1 second.
4294967295/(86400*100) = 497.10
```

**How we will do this:**

We can solve it by dividing sysUpTime value by 100 before producing a response, so it will produce the final value in seconds not in hundredth of a second. If you are going to set up new NMS then this approach will work. However It will create another issues if you do with existing NMS that We can not change agents in a one shot. Some agents will still run on old OID values (old sysUpTime). So our manager will get confused about how to interpret the time unit in seconds or in hundredths of a second.

- If you are setting up a new NMS who interprets sysUpTime in seconds not in hundreds of seconds this will work.
- If you less number of devices then you can change snmp agents and ignore the false alarm
- You can target a specific type of device and configure your NMS for these devices. But your manager should be capable of polling OIDs by device types.

My apologies to C Programmers, I am not a C Programmer, However I made some changes in the system.c file in Net-SNMP Package without knowing its side-effects.

**Code Change:**

Rename the *get_uptime()* function to *get_uptime_old()* and create a new function *get_uptime()* which will call *get_uptime_old()* and divide value by 100.
