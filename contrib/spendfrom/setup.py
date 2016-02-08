from distutils.core import setup
setup(name='btcspendfrom',
      version='1.0',
      description='Command-line utility for hodlcoin "coin control"',
      author='Gavin Andresen',
      author_email='gavin@hodlcoinfoundation.org',
      requires=['jsonrpc'],
      scripts=['spendfrom.py'],
      )
