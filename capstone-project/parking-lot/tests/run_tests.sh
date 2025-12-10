#!/bin/bash
# Run all tests

gcc -o ../bin/test_billing test_billing.c ../src/billing.c
../bin/test_billing
