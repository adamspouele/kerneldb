({
	"name": "Put",
	"author": "subprotocol",
	"description": "Always overwrites with the most recent value. Effectivly the same as Set().",
	"merge": function(key, old, value, log) {
		return value;
	},
	"partialMerge": function(key, left, right, log) {
		return right;
	}
});