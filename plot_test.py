from subprocess import Popen, PIPE
from multiprocessing import Pool

import numpy as np
import matplotlib.pyplot as plt
from path import path


def do_task(n):
    result = dict([l.strip().split(',')
            for l in Popen(['./test_blitz', '%d' % n], stdout=PIPE,
                    stderr=PIPE).communicate()[0].split('\n') if l.strip()])
    return n, result


def _parse_args():
    """Parses arguments, returns ``(options, args)``."""
    from argparse import ArgumentParser

    parser = ArgumentParser(description="""\
Plot runtimes using various storage strategies""",
                            epilog="""\
(C) 2012  Christian Fobel, licensed under the terms of GPL2.""",
                           )
    parser.add_argument('-o', '--output_path',
                    dest='output_path', default='blitz_performance.pdf',
                    type=path, help='PDF output path (default=%(default)s).')
    parser.add_argument('-p', '--process_count',
                    dest='process_count', default=None, type=int,
                    help='Number of parallel processes to run (default=%(default)s).')
    parser.add_argument('-n', '--max_item_2_exponent',
                    dest='max_item_2_exponent', default=24, type=int,
                    help='Max # of items to test, expressed as an exponent to '\
                            ' raise 2 to the power of (default=%(default)s).')
    args = parser.parse_args()

    if args.output_path.ext != '.pdf':
        parser.error('Output path must have extension ".pdf"')

    return args


if __name__ == '__main__':
    args = _parse_args()

    if args.process_count:
        pool = Pool(processes=args.process_count)
    else:
        pool = Pool()

    results = [pool.apply_async(do_task, [pow(2, i)])
            for i in range(10, args.max_item_2_exponent + 1)]
    pool.close()
    pool.join()
    unpacked_results = sorted([result.get() for result in results])
    np_results = np.array([[n] + [float(v) for v in zip(*sorted(times.items()))[1]]
            for n, times in unpacked_results])
    labels = ['n'] + sorted(times.keys())

    x_values = np_results[:, 0]
    for i in range(1, len(labels)):
        plt.plot(x_values, np_results[:, i], label=labels[i])
    plt.legend(loc=2)
    plt.savefig(args.output_path)
