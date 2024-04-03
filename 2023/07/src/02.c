#include <stdio.h>
#include "util.h"

const char *default_path = "input/07.txt";
const char *example_path = "input/example.txt";

#ifdef SILENT
#define fprintf(...) do { } while (0)
#endif

const char *determine_input_path(int argc, char **argv) {
	const char *input_path = default_path;
	if (argc > 1) {
		if (strncmp(argv[1], "-e", 2) == 0 ||
		    strncmp(argv[1], "--example", 9) == 0) {
			input_path = example_path;
		}
		if (strncmp(argv[1], "-f", 2) == 0 ||
		    strncmp(argv[1], "--file", 6) == 0) {
			if (argc == 2) {
				fprintf(stderr, "Option -f <file> requires an argument, none provided\n");
				exit(1);
			}
			input_path = argv[2];
		}
	}
	return input_path;
}

typedef enum {
	Card_Unknown = 0,
	Card_Joker = 1,
	Card_Two = 2,
	Card_Three = 3,
	Card_Four = 4,
	Card_Five = 5,
	Card_Six = 6,
	Card_Seven = 7,
	Card_Eight = 8,
	Card_Nine = 9,
	Card_Ten = 10,
	Card_Queen = 11,
	Card_King = 12,
	Card_Ace = 13,
} Card;

typedef enum {
	CPR_Unknown = 0,
	CPR_HighCard,
	CPR_OnePair,
	CPR_TwoPair,
	CPR_ThreeOfAKind,
	CPR_FullHouse,
	CPR_FourOfAKind,
	CPR_FiveOfAKind,
} CamelPokerResult;

StringView cpr_name(CamelPokerResult cpr) {
	StringView result = {0};
	switch (cpr) {
		case CPR_Unknown: {
			result.data = "unknown";
			result.count = sizeof("unknown")-1;
		} break;
		case CPR_HighCard: {
			result.data = "High Card";
			result.count = sizeof("High Card")-1;
		} break;
		case CPR_OnePair: {
			result.data = "One Pair";
			result.count = sizeof("One Pair")-1;
		} break;
		case CPR_TwoPair: {
			result.data = "Two Pairs";
			result.count = sizeof("Two Pairs")-1;
		} break;
		case CPR_ThreeOfAKind: {
			result.data = "Three of a Kind";
			result.count = sizeof("Three of a Kind")-1;
		}
		case CPR_FullHouse: {
			result.data = "Full House";
			result.count = sizeof("Full House")-1;
		} break;
		case CPR_FourOfAKind: {
			result.data = "Four of a Kind";
			result.count = sizeof("Four of a Kind")-1;
		} break;
		case CPR_FiveOfAKind: {
			result.data = "Five of a Kind";
			result.count = sizeof("Five of a Kind")-1;
		} break;
	}
	return result;
}

typedef struct Hand {
	Card cards[5];
	CamelPokerResult score;
	long bid;
} Hand;

StringView hand_print_cards(Hand hand) {
	StringView result = {0};
	result.data = malloc(5);
	result.count = 5;

	for (size_t i = 0; i < 5; i += 1) {
		switch (hand.cards[i]) {
			case Card_Joker: {
				result.data[i] = 'J';
			} break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9: {
				result.data[i] = '0' + hand.cards[i];
			} break;
			case Card_Ten: {
				result.data[i] = 'T';
			} break;
			case Card_Queen: {
				result.data[i] = 'Q';
			} break;
			case Card_King: {
				result.data[i] = 'K';
			} break;
			case Card_Ace: {
				result.data[i] = 'A';
			} break;
			case Card_Unknown: {
				result.data[i] = '?';
			} break;
		}
	}
	return result;
}

Hand string_view_parse_hand(StringView input) {
	Hand result = {0};
	if (input.count != 5) {
		fprintf(stderr, "The hand had %s 5 cards (%ld). Not cool.", input.count > 5 ? "more than" : "fewer than", input.count);
		return result;
	}

	int card_counter[13] = {0};

	int have_jokers = 0;
	for (size_t i = 0; i < input.count; i += 1) {
		Card card;
		switch (input.data[i]) {
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				card = input.data[i] - '0';
			} break;
			case 'T': {
				card = Card_Ten;
			} break;
			case 'J': {
				card = Card_Joker;
				have_jokers += 1;
			} break;
			case 'Q': {
				card = Card_Queen;
			} break;
			case 'K': {
				card = Card_King;
			} break;
			case 'A': {
				card = Card_Ace;
			} break;
		}
		card_counter[card-1] += 1;
		result.cards[i] = card;
	}

	// special case: 5 jokers. automatic 5 of a kind with lowest score
	if (card_counter[Card_Joker-1] == 5 || card_counter[Card_Joker-1] == 4) {
		result.score = CPR_FiveOfAKind;
	} else {
		int highest_match = 0, second_match = 0;
		int index_for_highest_match = 0;
		for (int i = 0; i < 13; i += 1) {
			if (i == Card_Joker - 1) { continue; }
			int matches = card_counter[i];
			if (matches > highest_match) {
				highest_match = matches;
				index_for_highest_match = i;
			}
		}

		for (int i = 0; i < 13; i += 1) {
			if (i == Card_Joker - 1) { continue; }
			if (i == index_for_highest_match) { continue; }
			int matches = card_counter[i];
			if (matches > second_match) {
				second_match = matches;
			}
		}

		if (highest_match == 5) {
			result.score = CPR_FiveOfAKind;
		} else if (highest_match == 4) {
			result.score = CPR_FourOfAKind;
		} else if (highest_match == 3) {
			if (second_match == 2) {
				result.score = CPR_FullHouse;
			} else {
				result.score = CPR_ThreeOfAKind;
			}
		} else if (highest_match == 2) {
			if (second_match == 2) {
				result.score = CPR_TwoPair;
			} else {
				result.score = CPR_OnePair;
			}
		} else if (highest_match == 1) {
			if (second_match == 1) {
				result.score = CPR_HighCard;
			}
		}

		/*
		Two Pair
		Three of a Kind
		Full House
		22233: No Jokers + Full House = Full House
		Four of a Kind
		22223: No Jokers + Four of a Kind = Four of a Kind
		2222J: 1 Joker + Four of a kidn = Five of a Kind
		*/
		if (have_jokers) {
			switch (result.score) {
				case CPR_HighCard: {
					/*
					Cases
					2345J: 1 Joker + High Card = One Pair
					234JJ: 2 Joker + High Card = Three of a Kind
					23JJJ: 3 Joker + High Card = Four of a Kind
					2JJJJ: 4 Joker + High Card = Five of a Kind
					*/
					if (have_jokers == 1) {
						result.score = CPR_OnePair;
					} else if (have_jokers == 2) {
						result.score = CPR_ThreeOfAKind;
					} else if (have_jokers == 3) {
						result.score = CPR_FourOfAKind;
					}
				} break;
				case CPR_OnePair: {
					/*
					2234J: 1 Joker + One Pair = Three of a Kind
					223JJ: 2 Joker + One Pair = Four of a Kind
					22JJJ: 3 Joker + One Pair = Five of a Kind
					*/
					if (have_jokers == 1) {
						result.score = CPR_ThreeOfAKind;
					} else if (have_jokers == 2) {
						result.score = CPR_FourOfAKind;
					} else if (have_jokers == 3) {
						result.score = CPR_FiveOfAKind;
					}
				} break;
				case CPR_TwoPair: {
					/*
					2233J: 1 Joker + Two Pair = Full House
					*/
					if (have_jokers == 1) {
						result.score = CPR_FullHouse;
					}
				} break;
				case CPR_ThreeOfAKind: {
					/*
					2223J: 1 Joker + Three of a Kind = Four of a Kind
					222JJ: 2 Joker + Three of a Kind = Five of a Kind
					*/
					if (have_jokers == 1) {
						result.score = CPR_FourOfAKind;
					} else if (have_jokers == 2) {
						result.score = CPR_FiveOfAKind;
					}
				} break;
				case CPR_FourOfAKind: {
					if (have_jokers == 1) {
						result.score = CPR_FiveOfAKind;
					}
				} break;
				case CPR_FullHouse:
				case CPR_FiveOfAKind: {
					// can't actually get here.
					// "You can't have six cards in a five card game." -Rusty Ryan
				} break;
				case CPR_Unknown: {
					// shouldn't actually get here.
				} break;
			}
		}
	}

	return result;
}

DynArray sort_hands_by_score(DynArray hands_da) {
	DynArray result = {0};

	size_t total_hands = hands_da.count;

	Hand *hands = dyn_array_as_type(hands_da, Hand);
	for (size_t i = 0; i < total_hands; i += 1) {
		size_t lowest_score_index = 0;
		Hand lowest_score_hand = hands[lowest_score_index];
		for (size_t j = 1; j < hands_da.count; j += 1) {
			Hand hand = hands[j];
			if (hand.score < lowest_score_hand.score) {
				lowest_score_index = j;
				lowest_score_hand = hand;
			} else if (hand.score == lowest_score_hand.score) {
				for (size_t card_index = 0; card_index < 5; card_index += 1) {
					if (hand.cards[card_index] < lowest_score_hand.cards[card_index]) {
						lowest_score_index = j;
						lowest_score_hand = hand;
						break;
					} else if (hand.cards[card_index] > lowest_score_hand.cards[card_index]) {
						break;
					}
				}
			}
		}
		dyn_array_add(&result, lowest_score_hand);
		dyn_array_remove(&hands_da, lowest_score_index, Hand);
	}

	return result;
}

int main(int argc, char **argv) {
	const char *input_path = determine_input_path(argc, argv);
	StringBuffer file;
	if (!read_entire_file(input_path, &file)) { return 1; }

	StringView content = {
		.data = file.data,
		.count = file.count,
	};

	DynArray lines = string_view_lines(content);
	StringView *lines_arr = dyn_array_as_type(lines, StringView);

	DynArray hands = {0};

	for (size_t i = 0; i < lines.count; i += 1) {
		StringView hand_sv = string_view_split_on(&lines_arr[i], ' ');
		Hand hand = string_view_parse_hand(hand_sv);

		DynArray bid_da = string_view_parse_numbers(lines_arr[i]);
		long bid = dyn_array_as_type(bid_da, long)[0];

		hand.bid = bid;
		dyn_array_add(&hands, hand);
	}

	DynArray hands_by_score = sort_hands_by_score(hands);
	Hand *hands_sorted = dyn_array_as_type(hands_by_score, Hand);

	long winnings_accumulator = 0;
	for (size_t i = 0; i < hands_by_score.count; i += 1) {
		Hand hand = hands_sorted[i];
		size_t rank = i + 1;
		long winnings = hand.bid * rank;
		fprintf(stdout, "%4ldx%-4ld on "SV_FMT": "SV_FMT" -> %ld\n", hand.bid, rank, SV_ARG(hand_print_cards(hand)), SV_ARG(cpr_name(hand.score)), winnings);
		winnings_accumulator += winnings;
		fprintf(stdout, "Accumlulator is now at %ld.\n", winnings_accumulator);
	}

	printf("Total winnings: %ld\n", winnings_accumulator);
	return 0;
}
