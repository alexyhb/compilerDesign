PARSER = ../parser

step: step1 step2 step3 step4 step5 step6 step7 step8

step1:
	$(PARSER) < step1.in | diff step1.out -

step2:
	$(PARSER) < step2.in | diff step2.out -
	$(PARSER) < step2-.in | diff syntax_error.out -

step3:
	$(PARSER) < step3.in | diff step3.out -
	$(PARSER) < step3-.in | diff syntax_error.out -

step4:
	$(PARSER) < step4.in | diff step4.out -

step5:
	$(PARSER) < step5.in | diff step5.out -

step6:
	$(PARSER) < step6.in | diff step6.out -