% Values for 1 inch lines 
K1=0.44;
offset1=0.39;

% Values for 2 inch lines 
K2=2.13;
offset2=0.23;

% Pulses
pulses=1:1:1000;
fact_litres=3.78541;

% Spec for 1 inch
discharge1=fact_litres*((pulses/60+offset1)*K1);

% Spec for 2 inch
discharge2=fact_litres*((pulses/60+offset2)*K2);

% Local calibration 1 inch
discharge1c=0.0407*pulses + 2.7478;

% Plot
hold on
plot(pulses,discharge1c,'b.');
plot(pulses,discharge1,'g.');
plot(pulses,discharge2,'k.');
legend('1-inch(cal)','1-inch','2-inch');
hold off

