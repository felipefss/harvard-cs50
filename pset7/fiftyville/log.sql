-- Keep a log of any SQL queries you execute as you solve the mystery.
-- Who the thief is,
-- What city the thief escaped to, and
-- Who the thief’s accomplice is who helped them escape
-- All you know is that the theft took place on July 28, 2020 and that it took place on Chamberlin Street.

-- =============== TODO: Erase all commands results =================

-- Checking the reports to see if there was a crime reported that specific date, at Chamberlin Street, and see if I can get more information
SELECT description FROM crime_scene_reports WHERE year=2020 AND month=7 AND day=28 AND street="Chamberlin Street";

-- As the crime report mentioned interviews, I thought it was a good idea to take a look at the interviews transcripts the witnesses gave
SELECT name,transcript FROM interviews WHERE year=2020 AND month=7 AND day=28 AND transcript LIKE "%courthouse%";

-- The query above returned 3 interviews, each of which gave different leads. For the first one, I looked for security footage of the courthouse within 10 minutes of the theft.
-- There were quite a few results from that, so no conclusion can be drawn just yet.
SELECT activity,hour,minute,license_plate FROM courthouse_security_logs WHERE year=2020 AND month=7 AND day=28 AND hour=10 AND minute>=15 AND minute<=25;

-- The second interview mentioned an ATM withdraw at Fifer Street, so I ran the next query to look for any information, which, again, proved to be inconclusive due to the amount of results.
-- It might be better to return later to these results
SELECT account_number FROM atm_transactions WHERE transaction_type="withdraw" AND year=2020 AND month=7 AND day=28 AND atm_location="Fifer Street";

-- Based on the third interview, I searched all phone calls taken that day with less than 1 minute duration.
SELECT caller,receiver,duration FROM phone_calls WHERE year=2020 AND month=7 AND day=28 AND duration<60;
-- The data gathered on the last 3 queries will be useful later to perform some JOIN queries and find the people responsible for the theft

-- The last interviewee also heard the thief asking their accomplice to buy a flight ticket for the next day leaving the city at the earliest.
-- I then search for flights on the day next to the theft, and queried for the minimum value of hour, which means the result will be the earliest flight.
SELECT id,destination_airport_id,hour,minute FROM flights WHERE year=2020 AND month=7 AND day=29 AND origin_airport_id=(SELECT id FROM airports WHERE city="Fiftyville") ORDER BY hour LIMIT 1;

-- Now that I've got the flight number, I can search for all the passengers on that flight, and query their passport numbers
SELECT passport_number FROM passengers WHERE flight_id=(SELECT id FROM flights WHERE year=2020 AND month=7 AND day=29 AND origin_airport_id=(SELECT id FROM airports WHERE city="Fiftyville") ORDER BY hour LIMIT 1);

-- We also need to check the bank account from which money was withdrawn in the morning prior to the theft.
-- The query below will return all the ID of owners of the bank accounts used in the morning of the theft to withdraw money
SELECT person_id FROM bank_accounts WHERE account_number IN (
    SELECT account_number FROM atm_transactions WHERE transaction_type="withdraw" AND year=2020 AND month=7 AND day=28 AND atm_location="Fifer Street");

-- Now I believe I gathered enough information to catch the thief.
-- So I performed a query looking for the people who have one of the license plates captured in the courthouse security feed; one of the passport numbers from the earliest flight leaving Fiftyville on the 29th
-- and their phone number is one of those who made a call less than 1 minute long on the day of the theft
SELECT name FROM people WHERE
	id IN (SELECT person_id FROM bank_accounts WHERE account_number IN
	    (SELECT account_number FROM atm_transactions WHERE transaction_type="withdraw" AND year=2020 AND month=7 AND day=28 AND atm_location="Fifer Street")) AND
    license_plate IN (SELECT license_plate FROM courthouse_security_logs WHERE year=2020 AND month=7 AND day=28 AND hour=10 AND minute>=15 AND minute<=25) AND
    phone_number IN (SELECT caller FROM phone_calls WHERE year=2020 AND month=7 AND day=28 AND duration<60) AND
    passport_number IN (SELECT passport_number FROM passengers WHERE flight_id=
        (SELECT id FROM flights WHERE year=2020 AND month=7 AND day=29 AND origin_airport_id=(SELECT id FROM airports WHERE city="Fiftyville") ORDER BY hour LIMIT 1));
-- The thief is Ernest

-- The destination city can be gotten from the flights information we found out long ago. Knowing which is the origin flight and the origin city, we have the information of the destination city.
SELECT city FROM airports WHERE id=(SELECT destination_airport_id FROM flights WHERE year=2020 AND month=7 AND day=29 AND origin_airport_id=(SELECT id FROM airports WHERE city="Fiftyville") ORDER BY hour LIMIT 1);

-- Finding out the thief's accomplice by searching who owns the phone number he called on the day of the theft:
SELECT name FROM people WHERE phone_number=
    (SELECT receiver FROM phone_calls WHERE year=2020 AND month=7 AND day=28 AND duration<60 AND caller=
    (SELECT phone_number FROM people WHERE id=686048));

-- Thus, solving the mystery.