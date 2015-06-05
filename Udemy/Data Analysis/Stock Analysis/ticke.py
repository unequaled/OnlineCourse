#!/usr/bin/env python
#
#

#
from datetime import datetime
import urllib2

# pandas and numpy
import numpy as np
import pandas as pd
from pandas import Series, DataFrame
from pandas import read_html
from pandas.io.data import DataReader




class ticker:
    def __init__(self, symbol):
        self.symbol = symbol
        
        if os.path.isfile(ticker_file_place(symbol)):
            pass
            #get data, update, and save
            
        else:
            pass
        
    def url_eps_data():
        '''
            To get the eps from website
        '''
        return 'https://ycharts.com/companies/' + self.symbol + '/eps'

    def ticker_file_place():
        '''
            To set the file location
        '''
        return './' + self.symbol + '.cvs'  
    
    def get_eps_table():
        '''
            To get the eps from website
        '''
        try:
            data = pd.io.html.read_html(url_eps_data(symbol))
            eps  = pd.concat([data[0].dropna(),data[1].dropna()]).reset_index(drop='True')
            eps  = eps.replace({'March':'Mar','Dec.':'Dec','Sept.':'Sep','June':'Jun'},regex=True)
            eps[0] = pd.to_datetime(eps[0], format='%b %d, %Y')     #This code transfer string to date object            
            eps.columns = ['Date','EPS']
            return eps
        
        except urllib2.HTTPError:
            return pd.DataFrame()
    
    def get_duration_date(eps_table):
        '''
            The eps_table date columns must be date object.
        '''
        return [eps_table.iloc[eps_table.shape[0]-1][0], eps_table.iloc[0][0]]

    def get_combined_talbe(eps_table):
        self.table = DataReader(self.symbol ,'yahoo',start,datetime.now())

if __name__ == '__Main__':
    pass
