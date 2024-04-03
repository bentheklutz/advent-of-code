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
	Card_Two = 2,
	Card_Three = 3,
	Card_Four = 4,
	Card_Five = 5,
	Card_Six = 6,
	Card_Seven = 7,
	Card_Eight = 8,
	Card_Nine = 9,
	Card_Ten = 10,
	Card_Jack = 11,
	Card_Queen = 12,
	Card_King = 13,
	Card_Ace = 14,
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

typedef struct Hand {
	Card cards[5];
	long score;
	long bid;
} Hand;

StringView hand_print_cards(Hand hand) {
	StringView result = {0};
	result.data = malloc(5);
	result.count = 5;

	for (size_t i = 0; i < 5; i += 1) {
		switch (hand.cards[i]) {
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
			case Card_Jack: {
				result.data[i] = 'J';
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
				card = Card_Jack;
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
		card_counter[card-2] += 1;
		result.cards[i] = card;
	}

	int highest_match = 0, second_match = 0;
	int index_for_highest_match = 0;
	for (int i = 0; i < 13; i += 1) {
		int matches = card_counter[i];
		if (matches > highest_match) {
			highest_match = matches;
			index_for_highest_match = i;
		}
	}

	for (int i = 0; i < 13; i += 1) {
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
		printf(SV_FMT": %ld -> %ld\n", SV_ARG(hand_print_cards(hand)), hand.score, winnings);
		winnings_accumulator += winnings;
	}

	printf("Total winnings: %ld\n", winnings_accumulator);
	return 0;
}
