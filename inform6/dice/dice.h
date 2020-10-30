System_file;

Include "Sorting";

#Ifndef LIBRARY_DICE;

Constant DICE_VERBOSE 	= true;

Constant DICE_DICE_LIMIT	= 32 * WORDSIZE - 1;
Constant DICE_SIDE_LIMIT	= 255;

Constant DICE_FUDGE_F	= -1;
Constant DICE_FUDGE_PLUS 	= 43;
Constant DICE_FUDGE_BLANK 	= 32;
Constant DICE_FUDGE_MINUS 	= 45;

Class Dice_GeometricRoll
	with rolls 	0 0 0 0 0 0 0 0
				0 0 0 0 0 0 0 0
				0 0 0 0 0 0 0 0
				0 0 0 0 0 0 0 0,
		dice,
		sides,
		modifier,
		keep_high,
		keep_low,
		keep_higher,
		keep_lower,
		keep_equals,
		versus,
		result,
		describe [;
			print self.dice, "d";
			switch (self.sides) {
				DICE_FUDGE_F:	print "F";
				100:			print "%";
				default:		print self.sides;
			}
			if (self._keeping()) {
				print " (", "keeping";
				if (self.keep_low > 0)	print " ", self.keep_low, " lowest";
				if (self.keep_high > 0)	print " ", self.keep_high, " highest";
				if (self.keep_higher > 0)	print " higher than ", self.keep_higher;
				if (self.keep_lower > 0)	print " lower than ", self.keep_lower;
				if (self.keep_equals > 0)	print " equal to ", self.keep_equals;
				print ")";
			}
			if (self.modifier > 0) print "+";
			if (self.modifier ~=0) print self.modifier;
			print_ret "";
		],
		call [i;
			if (DICE_VERBOSE) self.describe();
			self._roll();

			! Apply filters
			if (self._keeping()) {
				self._print_result();
				inssort_bytes(self.&rolls, 1, self.dice);

				for (i=1: i<=self.dice: i++) {
					if (self.keep_low  		&& i > self.keep_low) 					self.&rolls->i = 0;
					if (self.keep_high 		&& i < self.dice - self.keep_high + 1)	self.&rolls->i = 0;
					if (self.keep_higher	&& self.&rolls->i <= self.keep_higher)	self.&rolls->i = 0;
					if (self.keep_lower		&& self.&rolls->i >= self.keep_lower)	self.&rolls->i = 0;
					if (self.keep_equals	&& self.&rolls->i ~= self.keep_equals)	self.&rolls->i = 0;
				}

				inssort_bytes(self.&rolls, 1, self.dice);
			}

			! Calculate Final Result
			for (i=1: i<=self.dice: i++) {
				if (self._fudge()) {
					switch (self.&rolls->i) {
						DICE_FUDGE_PLUS:	self.result++;
						DICE_FUDGE_MINUS:	self.result--;
					}
				} else {
					self.result = self.result + self.&rolls->i;
				}
			}

			self.result = self.result + self.modifier;

			self._print_result(true);
		],
		_keeping [;
			return (
				self.keep_high ||
				self.keep_low ||
				self.keep_higher ||
				self.keep_lower ||
				self.keep_equals
			);
		],
		_fudge [;
			return self.sides == DICE_FUDGE_F;
		],
		_roll [i;
			self.result = 0;
			for (i=1: i<=self.dice: i++) {
				if (self._fudge()) {
					self.&rolls->i = random(DICE_FUDGE_PLUS, DICE_FUDGE_BLANK, DICE_FUDGE_MINUS);
				} else {
					self.&rolls->i = random(self.sides);
				}
			}
		],
		_print_result [final i;
			if (DICE_VERBOSE) {
				print "[";
				for (i=1: i<=self.dice: i++) {
					if (self._fudge()) {
						print (char) self.&rolls->i; if (i < self.dice) print ", ";
					} else if (self.&rolls->i ~= 0) {
						print self.&rolls->i; if (i < self.dice) print ", ";
					}
				}
				print "]";
				if (final) {
					if (self.modifier > 0) print "+";
					if (self.modifier ~=0) print self.modifier;
					print " = "; style bold; print self.result; style roman;
				}
			} else if (final) {
				style bold; print self.result; style roman;
			}
			print "^";
		];

Dice_GeometricRoll ad_hoc_roll;

[ Dice_AdHocRollSub;
	ad_hoc_roll.call();
];

[ Dice_RollForSub;

];

[ Dice_NumberOfDigits num digits;
	if (num == DICE_FUDGE_F) return 1;
	while(num > 0) {
		num = num / 10;
		digits++;
	}
	return digits;
];

[ Dice_NotationExtractNumber wa we digit digit_count num sign;
	sign = 1;
	for (num=0,digit_count=1 : wa<we : wa++,digit_count++) {
		switch (wa->0) {
			'+': if (num) {
					return num;
				} else {
					sign = 1;
				}
			'-': if (num) {
					return num;
				} else {
					sign = -1;
				}
			'0' to '9': digit = wa->0 - '0';
			'd', 'x', 'h', 'l', '<', '>', '=': return num;
			default: return GPR_FAIL;
		}
		num = 10 * num + digit;
	}
	return sign * num;
];

[ Dice_Notation wa we
	dice_num sides_num explode_num
	keep_high_num keep_low_num
	keep_higher_num keep_lower_num keep_equals_num
	modifier_num versus_num;
	
	if (wa == 0) {
		wa = WordAddress(wn);
		we = wa + WordLength(wn);
	}

	dice_num = Dice_NotationExtractNumber(wa, we);
	if (dice_num <= 0) return GPR_FAIL;
	wa = wa + Dice_NumberOfDigits(dice_num) + 1;
	if (dice_num > DICE_DICE_LIMIT) dice_num = DICE_DICE_LIMIT;

	if (wa->0 == '%') {
		sides_num = 100;
		wa++; wa++;
	} else if (wa->0 == 'f') {
		sides_num = DICE_FUDGE_F;
		wa++; wa++;
	} else {
		sides_num = Dice_NotationExtractNumber(wa, we);
		if (sides_num ~= DICE_FUDGE_F && sides_num <= 0) return GPR_FAIL;
		wa = wa + Dice_NumberOfDigits(sides_num) + 1;
		if (sides_num > DICE_SIDE_LIMIT) sides_num = DICE_SIDE_LIMIT;
	}

	wa = wa - 1;
	while (wa->0 == 'x' or 'h' or 'l' or '>' or '<' or '=') {
		switch(wa->0) {
			'x':
				wa++;
				explode_num = Dice_NotationExtractNumber(wa, we);
				if (explode_num <= 0) return GPR_FAIL;
				wa = wa + Dice_NumberOfDigits(explode_num) + 1;
			'h':
				wa++;
				keep_high_num = Dice_NotationExtractNumber(wa, we);
				if (keep_high_num <= 0) return GPR_FAIL;
				wa = wa + Dice_NumberOfDigits(keep_high_num) + 1;
			'l':
				wa++;
				keep_low_num = Dice_NotationExtractNumber(wa, we);
				if (keep_low_num <= 0) return GPR_FAIL;
				wa = wa + Dice_NumberOfDigits(keep_low_num) + 1;
			'>':
				wa++;
				keep_higher_num = Dice_NotationExtractNumber(wa, we);
				if (keep_higher_num <= 0) return GPR_FAIL;
				wa = wa + Dice_NumberOfDigits(keep_higher_num) + 1;
			'<':
				wa++;
				keep_lower_num = Dice_NotationExtractNumber(wa, we);
				if (keep_lower_num <= 0) return GPR_FAIL;
				wa = wa + Dice_NumberOfDigits(keep_lower_num) + 1;
			'=':
				wa++;
				keep_equals_num = Dice_NotationExtractNumber(wa, we);
				if (keep_equals_num <= 0) return GPR_FAIL;
				wa = wa + Dice_NumberOfDigits(keep_equals_num) + 1;
		}
		wa = wa - 1;
	}

	modifier_num = Dice_NotationExtractNumber(wa, we);
	wa = wa + Dice_NumberOfDigits(modifier_num) + 1;

	ad_hoc_roll.dice = dice_num;
	ad_hoc_roll.sides = sides_num;
	ad_hoc_roll.sides = explode_num;
	ad_hoc_roll.keep_high = keep_high_num;
	ad_hoc_roll.keep_low = keep_low_num;
	ad_hoc_roll.keep_higher = keep_higher_num;
	ad_hoc_roll.keep_lower = keep_lower_num;
	ad_hoc_roll.keep_equals = keep_equals_num;
	ad_hoc_roll.modifier = modifier_num;

	wn++;
	return GPR_NUMBER;
];

Verb 'roll'
	* Dice_Notation					->	Dice_AdHocRoll
	* Dice_Notation 'for' special	-> 	Dice_RollFor;

Constant LIBRARY_DICE;
#EndIf;

