package main

import (
	"fmt"
	"slices"
	"strings"
)

type guard_sim struct {
	width, height           int
	obstacles               []vec2
	guard_initial_position  vec2
	guard_position          vec2
	guard_initial_direction direction
	guard_direction         direction
	events                  []event
}

func (sim *guard_sim) copy() guard_sim {
	copy := guard_sim{}
	copy.width = sim.width
	copy.height = sim.height
	copy.obstacles = slices.Clone(sim.obstacles)
	copy.guard_initial_position = sim.guard_initial_position
	copy.guard_position = sim.guard_initial_position
	copy.guard_initial_direction = sim.guard_initial_direction
	copy.guard_direction = sim.guard_initial_direction
	copy.events = make([]event, 0)
	return copy
}

type vec2 struct {
	x, y int
}

func (v vec2) String() string {
	return fmt.Sprintf("(%d, %d)", v.x, v.y)
}

type direction int

const (
	unknown direction = iota
	up
	down
	left
	right
)

func (d direction) String() string {
	switch d {
	case up:
		return "up"
	case down:
		return "down"
	case left:
		return "left"
	case right:
		return "right"
	}
	return "unknown"
}

func rightFrom(d direction) direction {
	switch d {
	case up:
		return right
	case down:
		return left
	case left:
		return up
	case right:
		return down
	}
	return unknown
}

type event struct {
	type_           event_type
	start_position  vec2
	end_position    vec2
	start_direction direction
	end_direction   direction
}

type event_type int

const (
	et_unknown event_type = iota
	et_step
	et_turn
	et_exit
)

func (sim *guard_sim) String() string {
	output := make([]byte, (sim.width+1)*sim.height)
	for i := 0; i < len(output); i++ {
		output[i] = '.'
	}
	for i := sim.width; i < len(output); i += sim.width + 1 {
		output[i] = '\n'
	}
	for _, p := range sim.obstacles {
		output[p.x+p.y*(sim.width+1)] = '#'
	}

	for i := 0; i < len(sim.events); i++ {
		event := sim.events[i]
		p := event.start_position
		if event.type_ == et_turn {
			output[p.x+p.y*(sim.width+1)] = '+'
		}
		if event.type_ == et_step {
			if i > 0 && sim.events[i-1].type_ == et_turn {
				// don't overwrite the '+' from the turn
				continue
			}
			switch event.start_direction {
			case up:
				fallthrough
			case down:
				output[p.x+p.y*(sim.width+1)] = '|'
			case left:
				fallthrough
			case right:
				output[p.x+p.y*(sim.width+1)] = '-'
			}
		}
	}
	output[sim.guard_initial_position.x+sim.guard_initial_position.y*(sim.width+1)] = 'S'
	if sim.guard_position != sim.guard_initial_position {
		switch sim.guard_direction {
		case up:
			output[sim.guard_position.x+sim.guard_position.y*(sim.width+1)] = '^'
		case down:
			output[sim.guard_position.x+sim.guard_position.y*(sim.width+1)] = 'V'
		case left:
			output[sim.guard_position.x+sim.guard_position.y*(sim.width+1)] = '<'
		case right:
			output[sim.guard_position.x+sim.guard_position.y*(sim.width+1)] = '>'
		}
	}
	return string(output)
}

func newSimFromInput(contents []byte) (guard_sim, error) {
	lines := strings.Split(string(contents), "\n")
	lines = lines[:len(lines)-1]

	for i := 1; i < len(lines); i++ {
		if len(lines[i]) != len(lines[i-1]) {
			return guard_sim{}, fmt.Errorf("Line %d: The input map is not a square. This may be problematic.\n", i+1)
		}
	}
	sim := guard_sim{}
	sim.guard_direction = up
	sim.guard_position = vec2{}
	sim.guard_initial_position = vec2{}
	sim.guard_initial_direction = up
	sim.obstacles = make([]vec2, 0)
	sim.width = len(lines[0])
	sim.height = len(lines)

	guard_found := false
	for i := 0; i < len(lines); i++ {
		line := lines[i]
		for j := 0; j < len(line); j++ {
			pos := vec2{j, i}
			if line[j] == '#' {
				sim.obstacles = append(sim.obstacles, pos)
			}
			if line[j] == '^' {
				sim.guard_position.x = j
				sim.guard_position.y = i
				sim.guard_initial_position.x = j
				sim.guard_initial_position.y = i
				guard_found = true
			}
		}
	}

	if !guard_found {
		return guard_sim{}, fmt.Errorf("The input did not contain a guard '^'. The program cannot proceed.\n")
	}

	return sim, nil
}

func (sim *guard_sim) obstructed(pos vec2) bool {
	for _, obstacle := range sim.obstacles {
		if pos == obstacle {
			return true
		}
	}
	return false
}

func (sim *guard_sim) guard_in_room(pos vec2) bool {
	return pos.x >= 0 && pos.x < sim.width && pos.y >= 0 && pos.y < sim.height
}

func (sim *guard_sim) guard_exited() bool {
	return len(sim.events) > 0 && sim.events[len(sim.events)-1].type_ == et_exit
}

func (sim *guard_sim) reset() {
	sim.guard_direction = sim.guard_initial_direction
	sim.guard_position = sim.guard_initial_position
	sim.events = sim.events[:0]
}

func (sim *guard_sim) lastEvent() event {
	if len(sim.events) == 0 {
		return event{}
	}
	return sim.events[len(sim.events)-1]
}

func (sim *guard_sim) step() {
	if sim.guard_exited() {
		return
	}
	guard_in_room := true
	obstructed := false
	var next_pos vec2
	switch sim.guard_direction {
	case up:
		next_pos = vec2{sim.guard_position.x, sim.guard_position.y - 1}
		guard_in_room = sim.guard_in_room(next_pos)
		obstructed = sim.obstructed(next_pos)
	case down:
		next_pos = vec2{sim.guard_position.x, sim.guard_position.y + 1}
		guard_in_room = sim.guard_in_room(next_pos)
		obstructed = sim.obstructed(next_pos)
	case left:
		next_pos = vec2{sim.guard_position.x - 1, sim.guard_position.y}
		guard_in_room = sim.guard_in_room(next_pos)
		obstructed = sim.obstructed(next_pos)
	case right:
		next_pos = vec2{sim.guard_position.x + 1, sim.guard_position.y}
		guard_in_room = sim.guard_in_room(next_pos)
		obstructed = sim.obstructed(next_pos)
	}
	if !guard_in_room {
		sim.events = append(sim.events, event{et_exit, sim.guard_position, next_pos, sim.guard_direction, sim.guard_direction})
	} else if obstructed {
		switch sim.guard_direction {
		case up:
			sim.guard_direction = right
			sim.events = append(sim.events, event{et_turn, sim.guard_position, sim.guard_position, up, sim.guard_direction})
		case down:
			sim.guard_direction = left
			sim.events = append(sim.events, event{et_turn, sim.guard_position, sim.guard_position, down, sim.guard_direction})
		case left:
			sim.guard_direction = up
			sim.events = append(sim.events, event{et_turn, sim.guard_position, sim.guard_position, left, sim.guard_direction})
		case right:
			sim.guard_direction = down
			sim.events = append(sim.events, event{et_turn, sim.guard_position, sim.guard_position, right, sim.guard_direction})
		}
	} else {
		sim.events = append(sim.events, event{et_step, sim.guard_position, next_pos, sim.guard_direction, sim.guard_direction})
		sim.guard_position = next_pos
	}
}

func (sim *guard_sim) simulate() {
	for !sim.guard_exited() {
		sim.step()
	}
}

func (sim *guard_sim) count_visited_spaces() int {
	num_visited := 0

	visited := make(map[vec2]bool)
	for _, event := range sim.events {
		visited[event.start_position] = true
	}

	for range visited {
		num_visited += 1
	}

	return num_visited
}
