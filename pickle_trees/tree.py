import math
import random
import numpy as np
import re
import sys

sys.setrecursionlimit(99999)
SPLIT_CACHE = {}


class Rule:
    def __init__(self, priority, ranges):
        # each range is left inclusive and right exclusive, i.e., [left, right)
        self.priority = priority
        self.ranges = ranges
        self.names = ["src_ip", "dst_ip", "src_port", "dst_port", "proto"]

    def is_intersect(self, dimension, left, right):
        return not (left >= self.ranges[dimension*2+1] or \
            right <= self.ranges[dimension*2])

    def is_intersect_multi_dimension(self, ranges):
        for i in range(5):
            if ranges[i*2] >= self.ranges[i*2+1] or \
                    ranges[i*2+1] <= self.ranges[i*2]:
                return False
        return True

    def sample_packet(self):
        src_ip = random.randint(self.ranges[0], self.ranges[1] - 1)
        dst_ip = random.randint(self.ranges[2], self.ranges[3] - 1)
        src_port = random.randint(self.ranges[4], self.ranges[5] - 1)
        dst_port = random.randint(self.ranges[6], self.ranges[7] - 1)
        protocol = random.randint(self.ranges[8], self.ranges[9] - 1)
        packet = (src_ip, dst_ip, src_port, dst_port, protocol)
        assert self.matches(packet), packet
        return packet

    def matches(self, packet):
        assert len(packet) == 5, packet
        return self.is_intersect_multi_dimension([
            packet[0] + 0,  # src ip
            packet[0] + 1,
            packet[1] + 0,  # dst ip
            packet[1] + 1,
            packet[2] + 0,  # src port
            packet[2] + 1,
            packet[3] + 0,  # dst port
            packet[3] + 1,
            packet[4] + 0,  # protocol
            packet[4] + 1
        ])

    def is_covered_by(self, other, ranges):
        for i in range(5):
            if (max(self.ranges[i*2], ranges[i*2]) < \
                    max(other.ranges[i*2], ranges[i*2]))or \
                    (min(self.ranges[i*2+1], ranges[i*2+1]) > \
                    min(other.ranges[i*2+1], ranges[i*2+1])):
                return False
        return True

    def __str__(self):
        result = ""
        for i in range(len(self.names)):
            result += "%s:[%d, %d) " % (self.names[i], self.ranges[i * 2],
                                        self.ranges[i * 2 + 1])
        return result


def load_rules_from_file(file_name):
    rules = []
    rule_fmt = re.compile(r'^@(\d+).(\d+).(\d+).(\d+)/(\d+) '\
        r'(\d+).(\d+).(\d+).(\d+)/(\d+) ' \
        r'(\d+) : (\d+) ' \
        r'(\d+) : (\d+) ' \
        r'(0x[\da-fA-F]+)/(0x[\da-fA-F]+) ' \
        r'(.*?)')
    for idx, line in enumerate(open(file_name)):
        elements = line[1:-1].split('\t')
        line = line.replace('\t', ' ')

        sip0, sip1, sip2, sip3, sip_mask_len, \
        dip0, dip1, dip2, dip3, dip_mask_len, \
        sport_begin, sport_end, \
        dport_begin, dport_end, \
        proto, proto_mask = \
        (eval(rule_fmt.match(line).group(i)) for i in range(1, 17))

        sip0 = (sip0 << 24) | (sip1 << 16) | (sip2 << 8) | sip3
        sip_begin = sip0 & (~((1 << (32 - sip_mask_len)) - 1))
        sip_end = sip0 | ((1 << (32 - sip_mask_len)) - 1)

        dip0 = (dip0 << 24) | (dip1 << 16) | (dip2 << 8) | dip3
        dip_begin = dip0 & (~((1 << (32 - dip_mask_len)) - 1))
        dip_end = dip0 | ((1 << (32 - dip_mask_len)) - 1)

        if proto_mask == 0xff:
            proto_begin = proto
            proto_end = proto
        else:
            proto_begin = 0
            proto_end = 0xff

        rules.append(
            Rule(idx, [
                sip_begin, sip_end + 1, dip_begin, dip_end + 1, sport_begin,
                sport_end + 1, dport_begin, dport_end + 1, proto_begin,
                proto_end + 1
            ]))
    return rules


def to_bits(value, n):
    if value >= 2**n:
        print("WARNING: clamping value", value, "to", 2**n - 1)
        value = 2**n - 1
    assert value == int(value)
    b = list(bin(int(value))[2:])
    assert len(b) <= n, (value, b, n)
    return [0.0] * (n - len(b)) + [float(i) for i in b]


def onehot_encode(arr, n):
    out = []
    for a in arr:
        x = [0] * n
        for i in range(a):
            x[i] = 1
        out.extend(x)
    return out


class Node:
    def __init__(self, id, ranges, rules, depth, partitions, manual_partition):
        self.id = id
        self.partitions = list(partitions or [])
        self.manual_partition = manual_partition
        self.ranges = ranges
        self.rules = rules
        self.depth = depth
        self.children = []
        self.action = None
        self.pushup_rules = None
        self.num_rules = len(self.rules)

    def is_partition(self):
        """Returns if node was partitioned."""
        if not self.action:
            return False
        elif self.action[0] == "partition":
            return True
        elif self.action[0] == "cut":
            return False
        else:
            return False

    def match(self, packet):
        #partition
        # go through all children and only keep
        # the rule with minimum priority for each rule
        # returned by all its children
        if self.is_partition():
            matches = []
            for c in self.children:
                match = c.match(packet)
                if match:
                    matches.append(match)
            if matches:
                matches.sort(key=lambda r: self.rules.index(r))
                return matches[0]
            return None
        #cut 
        # if any of the children contains a packet, just return 
        # the what it matches. otherwise false.
        elif self.children:
            for n in self.children:
                if n.contains(packet):
                    return n.match(packet)
            return None
        #leaf
        # go through all the rules, and return the match.
        # this should always return 
        else:
            for r in self.rules:
                if r.matches(packet):
                    return r

    def is_intersect_multi_dimension(self, ranges):
        for i in range(5):
            if ranges[i*2] >= self.ranges[i*2+1] or \
                    ranges[i*2+1] <= self.ranges[i*2]:
                return False
        return True

    def contains(self, packet):
        assert len(packet) == 5, packet
        return self.is_intersect_multi_dimension([
            packet[0] + 0,  # src ip 0
            packet[0] + 1,
            packet[1] + 0,  # dst ip 2
            packet[1] + 1,
            packet[2] + 0,  # src port 4
            packet[2] + 1,
            packet[3] + 0,  # dst port 6
            packet[3] + 1,
            packet[4] + 0,  # protocol 8
            packet[4] + 1
        ])
    

    def field_as_binary(self, field, num):
        field = field // 2
        if field == 0:
            assert(num & 0x1ffffffff == num)
            return format(num, '033b')  # src ip
        if field == 1:
            assert(num & 0x1ffffffff == num)
            return format(num, '033b')  # dst ip
        if field == 2:
            assert(num & 0x1ffff == num)
            return format(num, '017b')  # src port
        if field == 3:
            assert(num & 0x1ffff == num)
            return format(num, '017b')  # dst port
        if field == 4:
            assert(num & 0x1ff == num)
            return format(num, '09b')  # protocol

    def range_as_binary(self, ranges, priority):
        result = ""
        assert(len(ranges) == 10)
        for i in [0, 4, 2, 6, 8, 1, 5, 3, 7, 9]:
            result += self.field_as_binary(i, ranges[i])
        result += format(priority, "032b")
        return result

    def as_binary(self, mapping, max_rules, max_children, rules):
        result = ""
        if self.is_partition():
            result += "00"
        elif self.children:
            result += "01"
        else:
            result += "10"
        result += self.range_as_binary(self.ranges, 0)
        range_width = len(self.range_as_binary(self.ranges, 0))
        if self.children:
            result += format(0, "032b")
            for i in range(max_rules):
                result += format(0, f"0{range_width}b")
        else:
            # is a leaf
            result += format(len(self.rules), "032b")
            for r in self.rules:
                result += self.range_as_binary(r.ranges, rules.index(r))
            for i in range(max_rules - len(self.rules)):
                result += format(0, f"0{range_width}b")
        result += format(len(self.children), "032b")
        for c in self.children:
            result += format(mapping[c.id], "032b")
        for i in range(max_children - len(self.children)):
            result += format(0, f"032b")
        if len(result) != range_width * (max_rules +1) + 2 + 64 + max_children * 32:
            print(f"max_r:{max_rules}, max_c:{max_children}, ")
            print(mapping[self.id])
            raise "ass"
        return result

    def is_useless(self):
        if not self.children:
            return False
        return max(len(c.rules) for c in self.children) == len(self.rules)

    def pruned_rules(self):
        new_rules = []
        for i in range(len(self.rules) - 1):
            rule = self.rules[len(self.rules) - 1 - i]
            flag = False
            for j in range(0, len(self.rules) - 1 - i):
                high_priority_rule = self.rules[j]
                if rule.is_covered_by(high_priority_rule, self.ranges):
                    flag = True
                    break
            if not flag:
                new_rules.append(rule)
        new_rules.append(self.rules[0])
        new_rules.reverse()
        return new_rules

    def get_state(self):
        state = []
        state.extend(to_bits(self.ranges[0], 32))
        state.extend(to_bits(self.ranges[1] - 1, 32))
        state.extend(to_bits(self.ranges[2], 32))
        state.extend(to_bits(self.ranges[3] - 1, 32))
        assert len(state) == 128, len(state)
        state.extend(to_bits(self.ranges[4], 16))
        state.extend(to_bits(self.ranges[5] - 1, 16))
        state.extend(to_bits(self.ranges[6], 16))
        state.extend(to_bits(self.ranges[7] - 1, 16))
        assert len(state) == 192, len(state)
        state.extend(to_bits(self.ranges[8], 8))
        state.extend(to_bits(self.ranges[9] - 1, 8))
        assert len(state) == 208, len(state)

        if self.manual_partition is None:
            # 0, 6 -> 0-64%
            # 6, 7 -> 64-100%
            partition_state = [
                0,
                7,  # [>=min, <max) -- 0%, 2%, 4%, 8%, 16%, 32%, 64%, 100%
                0,
                7,
                0,
                7,
                0,
                7,
                0,
                7,
            ]
            for (smaller, part_dim, part_size) in self.partitions:
                if smaller:
                    partition_state[part_dim * 2 + 1] = min(
                        partition_state[part_dim * 2 + 1], part_size + 1)
                else:
                    partition_state[part_dim * 2] = max(
                        partition_state[part_dim * 2], part_size + 1)
            state.extend(onehot_encode(partition_state, 7))
        else:
            partition_state = [0] * 70
            partition_state[self.manual_partition] = 1
            state.extend(partition_state)
        state.append(self.num_rules)
        return np.array(state)

    def __str__(self):
        result = "ID:%d\tAction:%s\tDepth:%d\tRange:\t%s\nChildren: " % (
            self.id, str(self.action), self.depth, str(self.ranges))
        for child in self.children:
            result += str(child.id) + " "
        result += "\nRules:\n"
        for rule in self.rules:
            result += str(rule) + "\n"
        if self.pushup_rules != None:
            result += "Pushup Rules:\n"
            for rule in self.pushup_rules:
                result += str(rule) + "\n"
        return result


class Tree:
    def __init__(
            self,
            rules,
            leaf_threshold,
            refinements={
                "node_merging": False,
                "rule_overlay": False,
                "region_compaction": False,
                "rule_pushup": False,
                "equi_dense": False
            }):
        # hyperparameters
        self.leaf_threshold = leaf_threshold
        self.refinements = refinements

        self.rules = rules
        self.root = self.create_node(
            0, [0, 2**32, 0, 2**32, 0, 2**16, 0, 2**16, 0, 2**8], rules, 1,
            None, None)
        if (self.refinements["region_compaction"]):
            self.refinement_region_compaction(self.root)
        self.current_node = self.root
        self.nodes_to_cut = [self.root]
        self.depth = 1
        self.node_count = 1

    def create_node(self, id, ranges, rules, depth, partitions,
                    manual_partition):
        node = Node(id, ranges, rules, depth, partitions, manual_partition)

        if self.refinements["rule_overlay"]:
            self.refinement_rule_overlay(node)

        return node

    def match(self, packet):
        return self.root.match(packet)

    def get_depth(self):
        return self.depth

    def get_current_node(self):
        return self.current_node

    def is_leaf(self, node):
        return len(node.rules) <= self.leaf_threshold

    def is_finish(self):
        return len(self.nodes_to_cut) == 0

    def update_tree(self, node, children):
        if self.refinements["node_merging"]:
            children = self.refinement_node_merging(children)

        if self.refinements["equi_dense"]:
            children = self.refinement_equi_dense(children)

        if (self.refinements["region_compaction"]):
            for child in children:
                self.refinement_region_compaction(child)

        node.children.extend(children)
        children.reverse()
        self.nodes_to_cut.pop()
        self.nodes_to_cut.extend(children)
        self.current_node = self.nodes_to_cut[-1]

    def partition_cutsplit(self):
        assert self.current_node is self.root
        from cutsplit import CutSplit
        self._split(self.root, CutSplit(self.rules), "cutsplit")

    def partition_efficuts(self):
        assert self.current_node is self.root
        from efficuts import EffiCuts
        self._split(self.root, EffiCuts(self.rules), "efficuts")

    def _split(self, node, splitter, name):
        key = (name, tuple(str(r) for r in self.rules))
        if key not in SPLIT_CACHE:
            print("Split not cached, recomputing")
            SPLIT_CACHE[key] = [
                p for p in splitter.separate_rules(self.rules) if len(p) > 0
            ]
        parts = SPLIT_CACHE[key]

        parts.sort(key=lambda x: -len(x))
        assert len(self.rules) == sum(len(s) for s in parts)
        print(splitter, [len(s) for s in parts])

        children = []
        for i, p in enumerate(parts):
            c = self.create_node(self.node_count, node.ranges, p,
                                 node.depth + 1, [], i)
            self.node_count += 1
            children.append(c)
        node.action = ("partition", 0, 0)
        self.update_tree(node, children)

    def partition_current_node(self, part_dim, part_size):
        return self.partition_node(self.current_node, part_dimension,
                                   part_size)

    def partition_node(self, node, part_dim, part_size):
        assert part_dim in [0, 1, 2, 3, 4], part_dim
        assert part_size in [0, 1, 2, 3, 4, 5], part_size
        self.depth = max(self.depth, node.depth + 1)
        node.action = ("partition", part_dim, part_size)

        def fits(rule, threshold):
            span = rule.ranges[part_dim * 2 + 1] - rule.ranges[part_dim * 2]
            assert span >= 0, rule
            return span < threshold

        small_rules = []
        big_rules = []
        max_size = [2**32, 2**32, 2**16, 2**16, 2**8][part_dim]
        threshold = max_size * 0.02 * 2**part_size  # 2% ... 64%
        for rule in node.rules:
            if fits(rule, threshold):
                small_rules.append(rule)
            else:
                big_rules.append(rule)

        left_part = list(node.partitions)
        left_part.append((True, part_dim, part_size))
        left = self.create_node(self.node_count, node.ranges, small_rules,
                                node.depth + 1, left_part, None)
        self.node_count += 1
        right_part = list(node.partitions)
        right_part.append((False, part_dim, part_size))
        right = self.create_node(self.node_count, node.ranges, big_rules,
                                 node.depth + 1, right_part, None)
        self.node_count += 1

        children = [left, right]
        self.update_tree(node, children)
        return children

    def cut_current_node(self, cut_dimension, cut_num):
        return self.cut_node(self.current_node, cut_dimension, cut_num)

    def cut_node(self, node, cut_dimension, cut_num):
        self.depth = max(self.depth, node.depth + 1)
        node.action = ("cut", cut_dimension, cut_num)
        range_left = node.ranges[cut_dimension * 2]
        range_right = node.ranges[cut_dimension * 2 + 1]
        range_per_cut = math.ceil((range_right - range_left) / cut_num)

        children = []
        assert cut_num > 0, (cut_dimension, cut_num)
        for i in range(cut_num):
            child_ranges = list(node.ranges)
            child_ranges[cut_dimension * 2] = range_left + i * range_per_cut
            child_ranges[cut_dimension * 2 + 1] = min(
                range_right, range_left + (i + 1) * range_per_cut)

            child_rules = []
            for rule in node.rules:
                if rule.is_intersect(cut_dimension,
                                     child_ranges[cut_dimension * 2],
                                     child_ranges[cut_dimension * 2 + 1]):
                    child_rules.append(rule)

            child = self.create_node(self.node_count, child_ranges,
                                     child_rules, node.depth + 1,
                                     node.partitions, node.manual_partition)
            children.append(child)
            self.node_count += 1

        self.update_tree(node, children)
        return children

    def cut_current_node_multi_dimension(self, cut_dimensions, cut_nums):
        self.depth = max(self.depth, self.current_node.depth + 1)
        node = self.current_node
        node.action = (cut_dimensions, cut_nums)

        range_per_cut = []
        for i in range(len(cut_dimensions)):
            range_left = node.ranges[cut_dimensions[i] * 2]
            range_right = node.ranges[cut_dimensions[i] * 2 + 1]
            cut_num = cut_nums[i]
            range_per_cut.append(
                math.ceil((range_right - range_left) / cut_num))

        cut_index = [0 for i in range(len(cut_dimensions))]
        children = []
        while True:
            # compute child ranges
            child_ranges = list(node.ranges)
            for i in range(len(cut_dimensions)):
                dimension = cut_dimensions[i]
                child_ranges[dimension*2] = node.ranges[dimension*2] + \
                    cut_index[i] * range_per_cut[i]
                child_ranges[dimension * 2 + 1] = min(
                    node.ranges[dimension * 2 + 1], node.ranges[dimension * 2]
                    + (cut_index[i] + 1) * range_per_cut[i])

            # compute child rules
            child_rules = []
            for rule in node.rules:
                if rule.is_intersect_multi_dimension(child_ranges):
                    child_rules.append(rule)

            # create new child
            child = self.create_node(self.node_count, child_ranges,
                                     child_rules, node.depth + 1,
                                     node.partitions, node.manual_partition)
            children.append(child)
            self.node_count += 1

            # update cut index
            cut_index[0] += 1
            i = 0
            while cut_index[i] == cut_nums[i]:
                cut_index[i] = 0
                i += 1
                if i < len(cut_nums):
                    cut_index[i] += 1
                else:
                    break

            if i == len(cut_nums):
                break

        self.update_tree(node, children)
        return children

    def cut_current_node_split(self, cut_dimension, cut_position):
        self.depth = max(self.depth, self.current_node.depth + 1)
        node = self.current_node
        node.action = (cut_dimension, cut_position)
        range_left = node.ranges[cut_dimension * 2]
        range_right = node.ranges[cut_dimension * 2 + 1]
        range_per_cut = cut_position - range_left

        children = []
        for i in range(2):
            child_ranges = node.ranges.copy()
            child_ranges[cut_dimension * 2] = range_left + i * range_per_cut
            child_ranges[cut_dimension * 2 + 1] = min(
                range_right, range_left + (i + 1) * range_per_cut)

            child_rules = []
            for rule in node.rules:
                if rule.is_intersect(cut_dimension,
                                     child_ranges[cut_dimension * 2],
                                     child_ranges[cut_dimension * 2 + 1]):
                    child_rules.append(rule)

            child = self.create_node(self.node_count, child_ranges,
                                     child_rules, node.depth + 1,
                                     node.partitions, node.manual_partition)
            children.append(child)
            self.node_count += 1

        self.update_tree(node, children)
        return children

    def get_next_node(self):
        self.nodes_to_cut.pop()
        if len(self.nodes_to_cut) > 0:
            self.current_node = self.nodes_to_cut[-1]
        else:
            self.current_node = None
        return self.current_node

    def check_contiguous_region(self, node1, node2):
        count = 0
        for i in range(5):
            if node1.ranges[i*2+1] == node2.ranges[i*2] or \
                    node2.ranges[i*2+1] == node1.ranges[i*2]:
                if count == 1:
                    return False
                else:
                    count = 1
            elif node1.ranges[i*2] != node2.ranges[i*2] or \
                    node1.ranges[i*2+1] != node2.ranges[i*2+1]:
                return False
        if count == 0:
            return False
        return True

    def merge_region(self, node1, node2):
        for i in range(5):
            node1.ranges[i * 2] = min(node1.ranges[i * 2], node2.ranges[i * 2])
            node1.ranges[i * 2 + 1] = max(node1.ranges[i * 2 + 1],
                                          node2.ranges[i * 2 + 1])

    def refinement_node_merging(self, nodes):
        while True:
            flag = True
            merged_nodes = [nodes[0]]
            last_node = nodes[0]
            for i in range(1, len(nodes)):
                if self.check_contiguous_region(last_node, nodes[i]):
                    if set(last_node.rules) == set(nodes[i].rules):
                        self.merge_region(last_node, nodes[i])
                        flag = False
                        continue

                merged_nodes.append(nodes[i])
                last_node = nodes[i]

            nodes = merged_nodes
            if flag:
                break

        return nodes

    def refinement_rule_overlay(self, node):
        if len(node.rules) == 0 or len(node.rules) > 500:
            return
        node.rules = node.pruned_rules()

    def refinement_region_compaction(self, node):
        if len(node.rules) == 0:
            return

        new_ranges = list(node.rules[0].ranges)
        for rule in node.rules[1:]:
            for i in range(5):
                new_ranges[i * 2] = min(new_ranges[i * 2], rule.ranges[i * 2])
                new_ranges[i * 2 + 1] = max(new_ranges[i * 2 + 1],
                                            rule.ranges[i * 2 + 1])
        for i in range(5):
            node.ranges[i * 2] = max(new_ranges[i * 2], node.ranges[i * 2])
            node.ranges[i * 2 + 1] = min(new_ranges[i * 2 + 1],
                                         node.ranges[i * 2 + 1])

    def refinement_rule_pushup(self):
        nodes_by_layer = [None for i in range(self.depth)]

        current_layer_nodes = [self.root]
        nodes_by_layer[0] = current_layer_nodes
        for i in range(self.depth - 1):
            next_layer_nodes = []
            for node in current_layer_nodes:
                next_layer_nodes.extend(node.children)
            nodes_by_layer[i + 1] = next_layer_nodes
            current_layer_nodes = next_layer_nodes

        for i in reversed(range(self.depth)):
            for node in nodes_by_layer[i]:
                if len(node.children) == 0:
                    node.pushup_rules = set(node.rules)
                else:
                    node.pushup_rules = set(node.children[0].pushup_rules)
                    for j in range(1, len(node.children)):
                        node.pushup_rules = node.pushup_rules.intersection(
                            node.children[j].pushup_rules)
                    for child in node.children:
                        child.pushup_rules = child.pushup_rules.difference(
                            node.pushup_rules)

    def refinement_equi_dense(self, nodes):
        # try to merge
        nodes_copy = []
        max_rule_count = -1
        for node in nodes:
            nodes_copy.append(
                Node(node.id, list(node.ranges), list(node.rules), node.depth,
                     node.partitions, node.manual_partition))
            max_rule_count = max(max_rule_count, len(node.rules))
        while True:
            flag = True
            merged_nodes = [nodes_copy[0]]
            last_node = nodes_copy[0]
            for i in range(1, len(nodes_copy)):
                if self.check_contiguous_region(last_node, nodes_copy[i]):
                    rules = set(last_node.rules).union(
                        set(nodes_copy[i].rules))
                    if len(rules) < len(last_node.rules) + len(nodes_copy[i].rules) and \
                        len(rules) < max_rule_count:
                        rules = list(rules)
                        rules.sort(key=lambda i: i.priority)
                        last_node.rules = rules
                        self.merge_region(last_node, nodes_copy[i])
                        flag = False
                        continue

                merged_nodes.append(nodes_copy[i])
                last_node = nodes_copy[i]

            nodes_copy = merged_nodes
            if flag:
                break

        # check condition
        if len(nodes_copy) <= 8:
            nodes = nodes_copy
        return nodes

    def compute_result(self):
        if self.refinements["rule_pushup"]:
            self.refinement_rule_pushup()

        # memory space
        # non-leaf: 2 + 16 + 4 * child num
        # leaf: 2 + 16 * rule num
        # details:
        #     header: 2 bytes
        #     region boundary for non-leaf: 16 bytes
        #     each child pointer: 4 bytes
        #     each rule: 16 bytes
        result = {"bytes_per_rule": 0, "memory_access": 0, \
            "num_leaf_node": 0, "num_nonleaf_node": 0, "num_node": 0}
        nodes = [self.root]
        while len(nodes) != 0:
            next_layer_nodes = []
            for node in nodes:
                next_layer_nodes.extend(node.children)

                # compute bytes per rule
                if self.is_leaf(node):
                    result["bytes_per_rule"] += 2 + 16 * len(node.rules)
                    result["num_leaf_node"] += 1
                else:
                    result["bytes_per_rule"] += 2 + 16 + 4 * len(node.children)
                    result["num_nonleaf_node"] += 1

            nodes = next_layer_nodes

        result["memory_access"] = self._compute_memory_access(self.root)
        result["bytes_per_rule"] = result["bytes_per_rule"] / len(self.rules)
        result[
            "num_node"] = result["num_leaf_node"] + result["num_nonleaf_node"]
        return result

    def _compute_memory_access(self, node):
        if self.is_leaf(node) or not node.children:
            return 1

        if node.is_partition():
            return sum(self._compute_memory_access(n) for n in node.children)
        else:
            return 1 + max(self._compute_memory_access(n) for n in node.children)

    def get_stats(self):
        widths = []
        dim_stats = []
        nodes = [self.root]
        while len(nodes) != 0 and len(widths) < 30:
            dim = [0] * 5
            next_layer_nodes = []
            for node in nodes:
                next_layer_nodes.extend(node.children)
                if node.action and node.action[0] == "cut":
                    dim[node.action[1]] += 1
            widths.append(len(nodes))
            dim_stats.append(dim)
            nodes = next_layer_nodes
        return {
            "widths": widths,
            "dim_stats": dim_stats,
        }

    def stats_str(self):
        stats = self.get_stats()
        out = "widths" + "," + ",".join(map(str, stats["widths"]))
        out += "\n"
        for i in range(len(stats["dim_stats"][0])):
            out += "dim{}".format(i) + "," + ",".join(
                str(d[i]) for d in stats["dim_stats"])
            out += "\n"
        return out

    def print_stats(self):
        print(self.stats_str())

    def print_layers(self, layer_num=5):
        nodes = [self.root]
        for i in range(layer_num):
            if len(nodes) == 0:
                return

            print("Layer", i)
            next_layer_nodes = []
            for node in nodes:
                print(node)
                next_layer_nodes.extend(node.children)
            nodes = next_layer_nodes

    def __str__(self):
        mapping = self.create_child_mapping()
        result = ""
        nodes = [self.root]
        print("Tree stats:")
        self.print_stats()
        print("Tree Nodes:")
        max_children = 0
        max_rules = 0
        result += "node; type; ranges; children:[]\n"
        while len(nodes) != 0:
            next_layer_nodes = []
            for node in nodes:
                if len(node.children) == 0:
                    max_rules = max(max_rules, len(node.rules))
                result += "%d; %s; %s; [" % (node.id, str(node.action),
                                             str(node.ranges))
                max_children = max(max_children, len(node.children))
                for child in node.children:
                    
                     result += str(child.id) + " "
                result += "]\n"
                next_layer_nodes.extend(node.children)
            nodes = next_layer_nodes
        result += f"max children: {max_children}\n"
        result += f"max rules: {max_rules}\n"
        return result

    def create_child_mapping(self):
        mapping = {}
        i = 0
        nodes = [self.root]
        while len(nodes) != 0:
            next_layer_nodes = []
            for node in nodes:
                mapping[node.id] = i
                i += 1
                for child in node.children:
                    pass
                next_layer_nodes.extend(node.children)
            nodes = next_layer_nodes
        return mapping


    def generate_nodes(self):
        mapping = self.create_child_mapping()
        total_nodes, total_rules, max_children, max_rules = self.generate_tree_constants()
        result = ""
        nodes = [self.root]
        while len(nodes) != 0:
            next_layer_nodes = []
            for node in nodes:
                result += node.as_binary(mapping, max_rules, max_children, self.rules) + "\n"
                max_children = max(max_children, len(node.children))
                for child in node.children:
                    pass
                next_layer_nodes.extend(node.children)
            nodes = next_layer_nodes
        return result

    def generate_tree_constants(self):
        nodes = [self.root]
        max_children = 0
        max_rules = 0
        total_nodes = 0
        total_rules = 0
        while len(nodes) != 0:
            next_layer_nodes = []
            for node in nodes:
                total_nodes += 1
                if len(node.children) == 0:
                    total_rules += 1
                    max_rules = max(max_rules, len(node.rules))
                max_children = max(max_children, len(node.children))
                for child in node.children:
                    pass
                    # result += str(child.id) + " "
                #result += "]\n"
                next_layer_nodes.extend(node.children)
            nodes = next_layer_nodes
        return (total_nodes, total_rules, max_children, max_rules)
    
    def generate_sv(self):
        total_nodes, total_rules, max_children, max_rules = self.generate_tree_constants()
        return f"""
package tree_constants;
    parameter TOTAL_NODES = {total_nodes};
    parameter TOTAL_RULES = {total_rules};
    parameter MAX_CHILDREN_PER_NODE = {max_children};
    parameter MAX_RULES_PER_NODE = {max_rules};
endpackage
"""
    
def check_classification(tree):
    failures = 0
    for i in range(10000):
        if i % 100 == 0:
            print("Testing randomly sampled packets", i)
        if random.random() > 0.5:
            packet = random.choice(tree.rules).sample_packet()
        else:
            packet = (random.randint(0, 2**32 - 1), random.randint(
                0, 2**32 - 1), random.randint(0, 2**16 - 1),
                      random.randint(0, 2**16 - 1), random.randint(
                          0, 2**5 - 1))
        packet = [126692207, 126711968, 2773, 24469, 1]
        expected_match = None
        for r in tree.rules:
            if r.matches(packet):
                expected_match = r
                break
        actual_match = tree.match(packet)
        expected_match = expected_match and tree.rules.index(expected_match)
        actual_match = actual_match and tree.rules.index(actual_match)
        if expected_match != actual_match:
            print("actual", actual_match, "expected", expected_match)
            failures += 1
    assert failures == 0, failures


import pickle

if __name__ == "__main__":
    #file = "./pickle_trees/fast_acl5_1k_tree.pkl"
    #file = "./pickle_trees/fw2_1k_nc.pkl"
    #file = "./pickle_trees/fw4_100k_nc.pkl"
    file = sys.argv[1]
    with open(file, "rb") as f:
        tree = pickle.load(f)
    with open("./tree_constants.sv", "w") as f:
        f.write(tree.generate_sv())
    with open("./nodes.txt", "w") as f:
        f.write(tree.generate_nodes())

    print(tree)
    check_classification(tree)
    print("All checks ok, this looks like a valid tree.")
